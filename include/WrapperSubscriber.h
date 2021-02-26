// WrapperSubscriber.h - Contains declarations of Subscriber wrapper of the Fast DDS Library
#pragma once

/*!
 * @file WrapperSubscriber.h
 * This header file contains the declaration class that wraps Fast DDS Library for 
 * building a Subscriber.
 */

#include "MessageTestPubSubTypes.h"

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

using namespace eprosima::fastdds::dds;

class WrapperSubscriber
{
    
public:
    /**
     * Constructor that sets the domain_Id 
     */
    WrapperSubscriber(int domain_id);

    ~WrapperSubscriber();

    /*
     * Initialize the Subscriber
     */
    bool init();

    /*
     * Run the subscriber
     */
    void run(uint32_t samples);

private:
    DomainParticipant* participant;

    int domain_id;

    Subscriber* subscriber;

    DataReader* reader;

    Topic* topic;

    TypeSupport type;

    class SubListener : public DataReaderListener
    {
    public:

        SubListener();

        ~SubListener() override;

	/**
	 * This method is called when a writer publishing in the same topic of the subscriber is discovered
	 */
        void on_subscription_matched(
               DataReader*,
               const SubscriptionMatchedStatus& info) override;

	/**
	 * This method is called when a new Data Message is received, retrieves the message and prints out the content 
	 */
        void on_data_available(
               DataReader* reader) override;

        MessageTest msgTest;

        std::atomic_int samples;

    } listener;

};
