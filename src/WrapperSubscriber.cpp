/**
 * @file WrapperSubscriber.cpp
 * This file contains the definitions of methods that wrap Fast DDS Library for 
 * building a Subscriber.
 *
 */

#include "MessageTestPubSubTypes.h"
#include "WrapperSubscriber.h"

#include <chrono>
#include <limits>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

using namespace eprosima::fastdds::dds;

typedef std::numeric_limits< double > dbl;

WrapperSubscriber::SubListener::SubListener()
        : samples(0)
    {
    }

WrapperSubscriber::SubListener::~SubListener() 
    {
    }

/*
 * This method is called when a writer publishing in the same topic of the subscriber is discovered 
 */
void WrapperSubscriber::SubListener::on_subscription_matched(
        DataReader*,
        const SubscriptionMatchedStatus& info)
   {
        if (info.current_count_change == 1)
        {
            std::cout << "Subscriber matched." << std::endl;
        }
        else if (info.current_count_change == -1)
        {
            std::cout << "Subscriber unmatched." << std::endl;
        }
        else
        {
            std::cout << info.current_count_change
                    << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
        }
    }
/*
 * This method is called when a new Data Message is received, retrieves the message and prints out the content 
 */
void WrapperSubscriber::SubListener::on_data_available(
          DataReader* reader) 
    {
        SampleInfo info;
        if (reader->take_next_sample(&msgTest, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                samples++;
		/*
		 * calculate the message latency
		 */
		// now
		auto current_time = std::chrono::system_clock::now();
	        auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
	        double num_seconds = duration_in_seconds.count();

		// started
		double startedTime = msgTest.now();

		double latency = num_seconds - startedTime;

		std::cout.precision(dbl::max_digits10);

                std::cout << "Message: " << msgTest.message() << " sent at: " << msgTest.now()
                            << " arrived at " <<  num_seconds << " RECEIVED." << std::endl;
		std::cout << "Latency: " << latency << " (seconds)" << std::endl;

            }
        }
    }

/**
 * Constructor that sets the domain_Id 
 */
WrapperSubscriber::WrapperSubscriber(int id)
        : participant(nullptr)
        , subscriber(nullptr)
        , topic(nullptr)
        , reader(nullptr)
        , type(new MessageTestPubSubType())
    {
        domain_id = id;
    }

WrapperSubscriber::~WrapperSubscriber()
    {
        if (reader != nullptr)
        {
            subscriber->delete_datareader(reader);
        }
        if (topic != nullptr)
        {
            participant->delete_topic(topic);
        }
        if (subscriber != nullptr)
        {
            participant->delete_subscriber(subscriber);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant);
    }

/*
 * Initialize the subscriber
 */
bool WrapperSubscriber::init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");
        participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant == nullptr)
        {
            return false;
        }

        // Register the Type
        type.register_type(participant);

        // Create the subscriptions Topic
        topic = participant->create_topic("MessageTestTopic", "MessageTest", TOPIC_QOS_DEFAULT);

        if (topic == nullptr)
        {
            return false;
        }

        // Create the Subscriber
        subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

        if (subscriber == nullptr)
        {
            return false;
        }

        // Create the DataReader
        reader = subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT, &listener);

        if (reader == nullptr)
        {
            return false;
        }

        return true;
    }

/*
 * Run the subscriber
 */
void WrapperSubscriber::run(
        uint32_t samples)
    {
        while(listener.samples < samples)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

