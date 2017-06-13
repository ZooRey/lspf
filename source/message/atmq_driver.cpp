#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/MessageListener.h>
#include <cms/ExceptionListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>


using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;

#include "boost/bind.hpp"
#include "atmq_driver.h"

#include "log/log.h"

namespace lspf {
namespace net {

class ATMQHandler
{
public:
    virtual ~ATMQHandler(){}

	virtual int Run() = 0;

	virtual void Close() = 0;

	virtual int SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> reply_addr) = 0;

    void SetHandle(int handle){
        m_handle = handle;
    }
    void SetOnMessage(const OnMessage &on_message){
        m_on_message = on_message;
    }
protected:
    int m_handle;
    OnMessage m_on_message;
};

class ATMQReply : public MessageReply{
public:
    ATMQReply(): destSend(NULL){};
    virtual ~ATMQReply(){
        //std::cout << "~ATMQReply" <<std::endl;
        if (destSend != NULL){
            delete destSend;
        }
    }

	std::string correlationID;

    //const cms::Destination* destSend;
    cms::Destination* destSend;
};


class ATMQClientHandler : public ATMQHandler,
						  public MessageListener,
						  public ExceptionListener,
						  public DefaultTransportListener {
public:

    ATMQClientHandler( const std::string& brokerURI, const std::string& destURI,
                    bool needReply = true, bool useTopic = false, bool clientAck = false );

    virtual ~ATMQClientHandler(){
        Close();
    }

    virtual int Run();
	// Called from the Producer since this class is a registered MessageListener.
	virtual void onMessage(const cms::Message* message);

    virtual void onException( const CMSException& ex AMQCPP_UNUSED );

    virtual void transportInterrupted();

    virtual void transportResumed();

    virtual void Close();

	virtual int SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> reply_addr);

private:
    Connection* connection;
    Session* session;

	Destination* tempDest;
    Destination* destination;

    MessageProducer* producer;
	MessageConsumer* consumer;

	bool needReply;
    bool useTopic;
    bool clientAck;
    std::string brokerURI;
    std::string destURI;
};

class ATMQServerHandler : public ATMQHandler,
						  public MessageListener,
						  public ExceptionListener,
						  public DefaultTransportListener {
public:

    ATMQServerHandler( const std::string& brokerURI, const std::string& destURI,
                    bool needReply = true, bool useTopic = false, bool clientAck = false );

    virtual ~ATMQServerHandler(){
        Close();
    }

    virtual int Run();
	// Called from the Producer since this class is a registered MessageListener.
	virtual void onMessage(const cms::Message* message);

    virtual void onException( const CMSException& ex AMQCPP_UNUSED );

    virtual void transportInterrupted();

    virtual void transportResumed();

    virtual void Close();

	virtual int SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> reply_addr);

private:
    Connection* connection;
    Session* session;

    Destination* destination;

    MessageProducer* producer;
	MessageConsumer* consumer;

	bool needReply;
    bool useTopic;
    bool clientAck;
    std::string brokerURI;
    std::string destURI;
};

ATMQClientHandler::ATMQClientHandler(const std::string& brokerURI,
									 const std::string& destURI,
									 bool needReply,
									 bool useTopic,
									 bool clientAck ) :
		connection(NULL),
		session(NULL),
		tempDest(NULL),
		destination(NULL),
		producer(NULL),
		consumer(NULL),
		needReply(needReply),
		useTopic(useTopic),
		clientAck(clientAck),
		brokerURI(brokerURI),
		destURI(destURI) {
}

void ATMQClientHandler::Close(){

     try{
        if( consumer != NULL ) delete consumer;
    }catch (CMSException& e) {}
    consumer = NULL;

    try{
        if( producer != NULL ) delete producer;
    }catch (CMSException& e) {}
    producer = NULL;

    // Destroy resources.
     try{
        if( tempDest != NULL ) delete tempDest;
    }catch (CMSException& e) {}
    tempDest = NULL;

    try{
        if( destination != NULL ) delete destination;
    }catch (CMSException& e) {}
    destination = NULL;

    // Close open resources.
    try{
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }catch (CMSException& e) {}

    // Now Destroy them
    try{
        if( session != NULL ) delete session;
    }catch (CMSException& e) {}
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch (CMSException& e) {}
    connection = NULL;
}

int ATMQClientHandler::Run(){
    try {
	    if (connection != NULL){
            return -1;
	    }

        // Create a ConnectionFactory
        boost::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
                            new ActiveMQConnectionFactory( brokerURI ) );

        // Create a Connection
        connection = connectionFactory->createConnection();

	    connection->start();

	    connection->setExceptionListener(this);

	    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
	    if( amqConnection != NULL ) {
	        amqConnection->addTransportListener( this );
	    }

        // Create a Session
        if( clientAck ) {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        // Create the destination (Topic or Queue)
        if( useTopic ) {
            destination = session->createTopic( destURI );
        } else {
            destination = session->createQueue( destURI );
        }

        // Create a MessageProducer from the Session to the Topic or Queue
        producer = session->createProducer( destination );
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

		if (!useTopic && needReply){
			//创建临时队列，用于接收应答
			tempDest = session->createTemporaryQueue();

			//应答消费对象
			consumer = session->createConsumer(tempDest);
			consumer->setMessageListener(this);//监听应答消息...
		}

    }catch ( CMSException& e ) {
        e.printStackTrace();
    }

	return 0;
}

void ATMQClientHandler::onMessage(const cms::Message* message){
    try {
          const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
          //ActiveMQMessageTransformation
          //std::auto_ptr<TextMessage> responsemessage(session->createTextMessage());
          //responsemessage->setCMSCorrelationID(textMessage->getCMSCorrelationID());
          //responsemessage->getCMSReplyTo()
          std::string text = "";
          if (textMessage != NULL) {
            text = textMessage->getText();
          } else {
            text = "NOT A TEXTMESSAGE!";
          }

          if( clientAck ) {
              message->acknowledge();
          }

          m_on_message(m_handle, text.c_str(), text.size(), boost::shared_ptr<MessageReply>());

          //producer.send
    } catch (CMSException& e) {
          printf("Producer onMessage() CMSException \n" );
          e.printStackTrace();
    }
}


int ATMQClientHandler::SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> addr){

    boost::shared_ptr<TextMessage> message(session->createTextMessage(std::string(msg, msg_len) ));

	if (!useTopic && needReply){
		std::string correlationId;

		message->setCMSReplyTo(tempDest);

		message->setCMSCorrelationID(correlationId);
	}

    message->setIntProperty( "Integer", 0 );

    // Tell the producer to send the message
    //printf( "Sent message %s\n", sendText.c_str() );
    producer->send( message.get() );

    return 0;
}

// If something bad happens you see it here as this class is also been
// registered as an ExceptionListener with the connection.
void ATMQClientHandler::onException( const CMSException& ex AMQCPP_UNUSED ) {
    PLOG_ERROR("CMS Exception occurred.  Shutting down client.\n");
    printf("CMS Exception occurred.  Shutting down client.\n");
}

void ATMQClientHandler::transportInterrupted() {
    PLOG_ERROR("ATMQClientHandler::transportInterrupted()");
}

void ATMQClientHandler::transportResumed() {
    PLOG_ERROR("ATMQClientHandler::transportResumed()");
}

//////////////////////////////////////////////////////////////////////////
ATMQServerHandler::ATMQServerHandler(const std::string& brokerURI,
									 const std::string& destURI,
									 bool needReply,
									 bool useTopic,
									 bool clientAck ) :
		connection(NULL),
		session(NULL),
		destination(NULL),
		producer(NULL),
		consumer(NULL),
		needReply(needReply),
		useTopic(useTopic),
		clientAck(clientAck),
		brokerURI(brokerURI),
		destURI(destURI) {
}


int ATMQServerHandler::Run() {

	try {
	    if (connection != NULL){
            return -1;
	    }
        // Create a ConnectionFactory
        boost::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
            new ActiveMQConnectionFactory( brokerURI ) );

        // Create a Connection
        connection = connectionFactory->createConnection();

        if (connection == NULL){
            std::cout << "createConnection failed" << std::endl;
            return -1;
        }

	    connection->start();
	    connection->setExceptionListener(this);
	    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
	    if( amqConnection != NULL ) {
            std::cout << "amqConnection != NULL" << std::endl;
	        amqConnection->addTransportListener( this );
	    }
	    // Create a Session
	    if( clientAck ) {
	        session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
	    } else {
	        session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
	    }
	    // Create the destination (Topic or Queue)
	    if( useTopic ) {
	        destination = session->createTopic( destURI );
	    } else {
	        destination = session->createQueue( destURI );
	    }

		if (!useTopic && needReply){
			// Create a MessageProducer from the Session to reply
			producer = session->createProducer();
			producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
		}
	    // Create a MessageConsumer from the Session to the Topic or Queue
	    consumer = session->createConsumer( destination );
	    consumer->setMessageListener( this );

	} catch (CMSException& e) {
	    e.printStackTrace();
	}

	return 0;
}

    // Called from the consumer since this class is a registered MessageListener.
void ATMQServerHandler::onMessage( const cms::Message* message ) {
    try
    {
        const TextMessage* textMessage =
            dynamic_cast< const TextMessage* >( message );
        std::string text = "";

        if( textMessage != NULL ) {
            text = textMessage->getText();
        } else {
            //text = "NOT A TEXTMESSAGE!";
            //记录错误消息日志
        }

        if( clientAck ) {
            message->acknowledge();
        }

		//int nProPerty=textMessage->getIntProperty("Integer");
		//printf("consumer Message #%d Received: %s,nProPerty[%d]\n", count, text.c_str(),nProPerty);

        boost::shared_ptr<ATMQReply> reply(new ATMQReply);

        reply->correlationID = textMessage->getCMSCorrelationID();

        const cms::Destination* destSend = textMessage->getCMSReplyTo();
        if (destSend != NULL){
            reply->destSend = (textMessage->getCMSReplyTo())->clone();
        }

		//消息回调处理
		m_on_message(m_handle, text.c_str(), text.size(), reply);

	} catch (CMSException& e) {
		e.printStackTrace();
    }
}


int ATMQServerHandler::SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> reply_addr){

	try{
	    std::string strReply(msg, msg_len);

	    boost::shared_ptr<ATMQReply> reply = boost::dynamic_pointer_cast<ATMQReply>(reply_addr);

		boost::shared_ptr<TextMessage> rspMessage(session->createTextMessage( strReply ));

		rspMessage->setCMSCorrelationID(reply->correlationID);

		const cms::Destination* destSend = reply->destSend;
		if(destSend)
		{
			this->producer->send(destSend, rspMessage.get());
		}

	} catch (CMSException& e) {
		e.printStackTrace();
    }

	return 0;
}

// If something bad happens you see it here as this class is also been
// registered as an ExceptionListener with the connection.
void ATMQServerHandler::onException( const CMSException& ex AMQCPP_UNUSED ) {
    PLOG_ERROR("CMS Exception occurred.  Shutting down client.\n:");
    printf("CMS Exception occurred.  Shutting down client.\n:");
    //exit(1);
}

void ATMQServerHandler::transportInterrupted() {
    PLOG_ERROR("ATMQServerHandler::transportInterrupted()");
}

void ATMQServerHandler::transportResumed() {
    PLOG_ERROR("ATMQServerHandler::transportResumed()");
}

void ATMQServerHandler::Close(){

    try{
        if( consumer != NULL ) delete consumer;
    }catch (CMSException& e) {}
    consumer = NULL;

    try{
        if( producer != NULL ) delete producer;
    }catch (CMSException& e) {}
    producer = NULL;
    // Destroy resources.
    try{
        if( destination != NULL ) delete destination;
    }catch (CMSException& e) {}
    destination = NULL;

    // Close open resources.
    try{
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }catch (CMSException& e) {}

    // Now Destroy them
    try{
        if( session != NULL ) delete session;
    }catch (CMSException& e) {}
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch (CMSException& e) {}
    connection = NULL;
}


ATMQDriver::ATMQDriver( const std::string& brokerURI,
						const std::string& destURI,
						bool needReply,
						bool useTopic,
						bool clientAck ):
		m_needReply(needReply),
		m_useTopic(useTopic),
		m_clientAck(clientAck),
		m_brokerURI(brokerURI),
		m_destURI(destURI),
		m_atmq_handler(NULL){

}

ATMQDriver::~ATMQDriver(){
	if (m_atmq_handler != NULL){
		delete m_atmq_handler;
	}
}

int ATMQDriver::Init(DRIVER_TYPE type){
     if (m_atmq_handler != NULL){
        return 0;
    }

    switch (type){
    case MESSAGE_CLIENT:
        m_atmq_handler = new ATMQClientHandler(m_brokerURI, m_destURI, m_needReply, m_useTopic, m_clientAck);
        break;
    case MESSAGE_SERVER:
        m_atmq_handler = new ATMQServerHandler(m_brokerURI, m_destURI, m_needReply, m_useTopic, m_clientAck);
        break;
    default:
        m_atmq_handler = new ATMQClientHandler(m_brokerURI, m_destURI, m_needReply, m_useTopic, m_clientAck);
        break;
    }

    if (m_atmq_handler == NULL){
        return -1;
    }

    return 0;
}

int ATMQDriver::Run(int handle, SendToFunction *send_to){

    m_atmq_handler->SetHandle(handle);

    m_atmq_handler->SetOnMessage(m_on_message);

	m_atmq_handler->Run();

	*send_to = boost::bind(&ATMQDriver::SendTo, this, _1, _2, _3);

	return 0;
}

int ATMQDriver::SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> reply_addr)
{
    m_atmq_handler->SendTo(msg, msg_len, reply_addr);

    return 0;
}


int ATMQDriver::Close(){
	m_atmq_handler->Close();
	return 0;
}

void ATMQDriver::Install(){
    activemq::library::ActiveMQCPP::initializeLibrary();
}

void ATMQDriver::Uninstall(){
    activemq::library::ActiveMQCPP::shutdownLibrary();
}

} /* namespace net */
} /* namespace lspf */

