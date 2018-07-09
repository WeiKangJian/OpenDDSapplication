#include "S01_SupermarketTypeSupportImpl.h"
#include "DataReaderListenerImpl.h"
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/WaitSet.h>
//#include <dds/DCPS/SubscriberImpl.h>
//#include <dds/DCPS/BuiltInTopicUtils.h>
#include <ace/streams.h>
//#include <ace/OS_NS_unistd.h>
#include <ace/Log_Msg.h>
#include <dds/DCPS/StaticIncludes.h>


int main(int argc, char *argv[])
{
	try {
		//初始化域参数工厂
		DDS::DomainParticipantFactory_var dpf =
			TheParticipantFactoryWithArgs(argc, argv);
		//创建域参数
		DDS::DomainParticipant_var participant =
			dpf->create_participant(42, // Domain ID
				PARTICIPANT_QOS_DEFAULT,
				0,  // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//域参数创建异常处理
		if (!participant) {
			std::cerr << "create_participant failed." << std::endl;
			return 1;
		}
		//注册类型支持类
		S01_Supermarket::DefaultInfTypeSupport_var mts = new S01_Supermarket::DefaultInfTypeSupportImpl;
		//支持类异常处理
		if (DDS::RETCODE_OK != mts->register_type(participant, "")) {
			std::cerr << "Failed to register the DefaultInfTypeSupport." << std::endl;
			return 1;
		}
		//创建主题2（同样也是DefaultInf S01_Supermarket）
		CORBA::String_var type_name = mts->get_type_name();
		DDS::Topic_var topic2 =
			participant->create_topic("DefaultInf S01_Supermarket",
				type_name,
				TOPIC_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//主题异常处理
		if (!topic2) {
			std::cerr << "Failed to create_topic." << std::endl;
			return 1;
		}
		//创建主题（同样也是DefaultInf S01_Supermarket）
		DDS::Topic_var topic =
			participant->create_topic("UpdateInf S01_Supermarket",
				type_name,
				TOPIC_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//主题异常处理
		if (!topic) {
			std::cerr << "Failed to create_topic." << std::endl;
			return 1;
		}

		//创建订阅者
		DDS::Subscriber_var sub =
			participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//异常处理
		if (!sub) {
			std::cerr << "Failed to create_subscriber." << std::endl;
			return 1;
		}

		cout << "输入0时只接受美团外卖主题，输入1时只接受饿了么主题，输入其他数字将同时接收这两个主题：" << endl;
		int a;
		scanf("%d",&a);
		if (a == 0)
		{
			DDS::DataReaderListener_var listener(new DataReaderListenerImpl);
			DDS::DataReader_var dr =
				sub->create_datareader(topic,
					DATAREADER_QOS_DEFAULT,
					listener,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			//读者创建异常处理
			if (!dr) {
				std::cerr << "create_datareader failed." << std::endl;
				return 1;
			}
			S01_Supermarket::DefaultInfDataReader_var reader_i = S01_Supermarket::DefaultInfDataReader::_narrow(dr);
			if (!reader_i)
			{
				std::cerr << "_narrow failed." << std::endl;
			}
			//等待集设置
			DDS::StatusCondition_var condition2 = dr->get_statuscondition();
			condition2->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
			DDS::WaitSetInterf_var ws2 = new DDS::WaitSet;
			ws2->attach_condition(condition2);
			while (true)
			{
				DDS::SubscriptionMatchedStatus matches;
				if (dr->get_subscription_matched_status(matches) != DDS::RETCODE_OK)
				{
					std::cerr << "get_subscription_matched_status failed!" << std::endl;
					return 1;
				}
				if (matches.current_count == 0 && matches.total_count > 0)
				{
					break;
				}
				DDS::ConditionSeq conditions;
				DDS::Duration_t timeout = { 200, 0 };
				if (ws2->wait(conditions, timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait failed." << std::endl;
					return 1;
				}
			}
			ws2->detach_condition(condition2);
		}
		else if (a == 1)
		{
			//创建数据读者2
			DDS::DataReaderListener_var listener2(new DataReaderListenerImpl);
			DDS::DataReader_var dr2 =
				sub->create_datareader(topic2,
					DATAREADER_QOS_DEFAULT,
					listener2,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			if (!dr2) {
				std::cerr << "create_datareader2 failed." << std::endl;
				return 1;
			}
			S01_Supermarket::DefaultInfDataReader_var reader_i2 = S01_Supermarket::DefaultInfDataReader::_narrow(dr2);
			if (!reader_i2)
			{
				std::cerr << "2_narrow failed." << std::endl;
			}
			//等待集设置
			DDS::StatusCondition_var condition = dr2->get_statuscondition();
			condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
			DDS::WaitSetInterf_var ws = new DDS::WaitSet;
			ws->attach_condition(condition);
			while (true)
			{
				DDS::SubscriptionMatchedStatus matches;
				if (dr2->get_subscription_matched_status(matches) != DDS::RETCODE_OK)
				{
					std::cerr << "get_subscription_matched_status failed!" << std::endl;
					return 1;
				}
				if (matches.current_count == 0 && matches.total_count > 0)
				{
					break;
				}
				DDS::ConditionSeq conditions;
				DDS::Duration_t timeout = { 200, 0 };
				if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait failed." << std::endl;
					return 1;
				}
			}
			ws->detach_condition(condition);
		}
		else
		{	
			//创建数据读者2
			DDS::DataReaderListener_var listener2(new DataReaderListenerImpl);
			DDS::DataReader_var dr2 =
				sub->create_datareader(topic2,
					DATAREADER_QOS_DEFAULT,
					listener2,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			if (!dr2) {
				std::cerr << "create_datareader2 failed." << std::endl;
				return 1;
			}
			DDS::DataReaderListener_var listener(new DataReaderListenerImpl);
			DDS::DataReader_var dr =
				sub->create_datareader(topic,
					DATAREADER_QOS_DEFAULT,
					listener,
					OpenDDS::DCPS::DEFAULT_STATUS_MASK);
			//读者创建异常处理
			if (!dr) {
				std::cerr << "create_datareader failed." << std::endl;
				return 1;
			}
			S01_Supermarket::DefaultInfDataReader_var reader_i = S01_Supermarket::DefaultInfDataReader::_narrow(dr);
			if (!reader_i)
			{
				std::cerr << "_narrow failed." << std::endl;
			}
			S01_Supermarket::DefaultInfDataReader_var reader_i2 = S01_Supermarket::DefaultInfDataReader::_narrow(dr2);
			if (!reader_i2)
			{
				std::cerr << "2_narrow failed." << std::endl;
			}
			//等待集设置
			DDS::StatusCondition_var condition2 = dr->get_statuscondition();
			condition2->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
			DDS::WaitSetInterf_var ws2 = new DDS::WaitSet;
			ws2->attach_condition(condition2);
			while (true)
			{
				DDS::SubscriptionMatchedStatus matches;
				if (dr->get_subscription_matched_status(matches) != DDS::RETCODE_OK)
				{
					std::cerr << "get_subscription_matched_status failed!" << std::endl;
					return 1;
				}
				if (matches.current_count == 0 && matches.total_count > 0)
				{
					break;
				}
				DDS::ConditionSeq conditions;
				DDS::Duration_t timeout = { 200, 0 };
				if (ws2->wait(conditions, timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait failed." << std::endl;
					return 1;
				}
			}
			ws2->detach_condition(condition2);
			//等待集设置
			DDS::StatusCondition_var condition = dr2->get_statuscondition();
			condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
			DDS::WaitSetInterf_var ws = new DDS::WaitSet;
			ws->attach_condition(condition);
			while (true)
			{
				DDS::SubscriptionMatchedStatus matches;
				if (dr2->get_subscription_matched_status(matches) != DDS::RETCODE_OK)
				{
					std::cerr << "get_subscription_matched_status failed!" << std::endl;
					return 1;
				}
				if (matches.current_count == 0 && matches.total_count > 0)
				{
					break;
				}
				DDS::ConditionSeq conditions;
				DDS::Duration_t timeout = { 200, 0 };
				if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait failed." << std::endl;
					return 1;
				}
			}
			ws->detach_condition(condition);
		}
		//清理
		participant->delete_contained_entities();
		dpf->delete_participant(participant);
		TheServiceParticipant->shutdown();
	}
	catch (CORBA::Exception& e) {
		cerr << "Exception caught in main.cpp:" << endl
			<< e << endl;
		ACE_OS::exit(1);
	}
	return 0;
}