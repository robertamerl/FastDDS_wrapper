/**
 * @file WrapperPublisher.cpp
 * This file contains the definitions of methods that wrap Fast DDS Library for 
 * building a Publisher.
 *
 */

#include "MessageTestPubSubTypes.h"
#include "WrapperPublisher.h"

#include <chrono>
#include <limits>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

using namespace eprosima::fastdds::dds;

typedef std::numeric_limits< double > dbl;

/**
 * Constructor that sets the domain_Id 
 */
WrapperPublisher::WrapperPublisher(int id)
        : participant(nullptr)
        , publisher(nullptr)
        , topic(nullptr)
        , writer(nullptr)
        , type(new MessageTestPubSubType())
    {
    	domain_id = id;
    }

WrapperPublisher::~WrapperPublisher()
    {
        if (writer != nullptr)
        {
            publisher->delete_datawriter(writer);
        }
        if (publisher != nullptr)
        {
            participant->delete_publisher(publisher);
        }
        if (topic != nullptr)
        {
            participant->delete_topic(topic);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant);
    }



WrapperPublisher::PubListener::PubListener()
            : matched(0)
        {
        }

WrapperPublisher::PubListener::~PubListener()
        {
        }
/**
 * The method is called when a publisher matches (or unmatches) a subscriber on the same endpoint 
 */
void WrapperPublisher::PubListener::on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info)
        {
            if (info.current_count_change == 1)
            {
                matched = info.total_count;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

/*
 * Initialize the publisher
 */
bool WrapperPublisher::init()
  {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant == nullptr)
        {
            return false;
        }

        // Register the Type
        type.register_type(participant);

        // Create the publications Topic
        topic = participant->create_topic("MessageTestTopic", "MessageTest", TOPIC_QOS_DEFAULT);

        if (topic == nullptr)
        {
            return false;
        }

        // Create the Publisher
        publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

        if (publisher == nullptr)
        {
            return false;
        }

        // Create the DataWriter
        writer = publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT, &listener);

        if (writer == nullptr)
        {
            return false;
        }

        return true;
 }

/*
 * Publish the message on the topic
 */
bool WrapperPublisher::publish(MessageTest* message)
    {
        if (listener.matched > 0)
        {
	    auto current_time = std::chrono::system_clock::now();
	    auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
	    double num_seconds = duration_in_seconds.count();

	    //std::cout.precision(dbl::max_digits10);
	    //std::cout << "Before message published - time: " << num_seconds << " seconds \n";

            message->now(num_seconds);

            writer->write(message);

            return true;
        }
        return false;
    }

/*
 * Run the Publisher
 */
void WrapperPublisher::run(
            uint32_t samples, MessageTest* message)
    {
        uint32_t samples_sent = 0;
        while (samples_sent < samples)
        {
            if (publish(message))
            {
                samples_sent++;
		std::cout.precision(dbl::max_digits10);
                std::cout << "Message: " << message->message() << " with  time: " << message->now()
                            << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

