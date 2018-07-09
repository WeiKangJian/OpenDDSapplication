#include "YH03TypeSupportImpl.h"
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

ofstream outf;
int main(int argc, char *argv[])
{
	outf.open("报告.txt");
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
		YH03::ObserveTypeSupport_var mts = new YH03::ObserveTypeSupportImpl;
		//支持类异常处理
		if (DDS::RETCODE_OK != mts->register_type(participant, "")) {
			std::cerr << "Failed to register the ObserveTypeSupport." << std::endl;
			return 1;
		}
		//创建主题（同样也是Observe YH03）
		CORBA::String_var type_name = mts->get_type_name();
		DDS::Topic_var topic =
			participant->create_topic("Observe YH03",
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
		//创建数据读者
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
		//类型窄化
		YH03::ObserveDataReader_var reader_i = YH03::ObserveDataReader::_narrow(dr);
		if (!reader_i)
		{
			std::cerr << "_narrow failed." << std::endl;
		}
		//等待集设置
		DDS::StatusCondition_var condition = dr->get_statuscondition();
		condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
		DDS::WaitSetInterf_var ws = new DDS::WaitSet;
		ws->attach_condition(condition);
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
			DDS::Duration_t timeout = { 60, 0 };
			if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
			{
				std::cerr << "wait failed." << std::endl;
				return 1;
			}
		}
		ws->detach_condition(condition);
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