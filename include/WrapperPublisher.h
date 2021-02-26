// WrapperPublisher.h - Contains declarations of Publish wrapper of the Fast DDS Library
#pragma once

/*!
 * @file WrapperPublisher.h
 * This header file contains the declaration of the class that wraps Fast DDS Library for 
 * building a Publisher.
 */

#include "MessageTestPubSubTypes.h"

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

using namespace eprosima::fastdds::dds;

class WrapperPublisher
{
    
public:
    /**
     * Constructor that sets the domain_Id 
     */
    WrapperPublisher(int domain_id);

    ~WrapperPublisher();

    /*
     * Initialize the publisher
     */
    bool init();

    /*
     * Publish the message on the topic
     */
    bool publish(MessageTest* message);

    /*
     * Run the publisher
     */
    void run(uint32_t samples, MessageTest* message);

private:
    DomainParticipant* participant;

    Publisher* publisher;

    Topic* topic;

    DataWriter* writer;

    int domain_id;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener();

        ~PubListener() override;

	/*
	 * The method is called when a publisher matches (or unmatches) a subscriber on the same endpoint
	 */
        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override;

        std::atomic_int matched;

    } listener; 

    TypeSupport type;
};
