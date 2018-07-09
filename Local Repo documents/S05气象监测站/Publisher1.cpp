#include "YH03TypeSupportImpl.h"
#include <dds\DdsDcpsInfrastructureC.h>
#include <dds\DdsDcpsPublicationC.h>
#include <dds\DCPS\StaticIncludes.h>
#include <dds\DCPS\Service_Participant.h>
#include <dds\DCPS\Marked_Default_Qos.h>
//#include <dds\DCPS\PublisherImpl.h>
#include <dds\DCPS\WaitSet.h>
#include <ace\streams.h>
//#include <ace\OS_NS_unistd.h>
#include <ace\Log_Msg.h>
#include <sys/timeb.h>
#include<time.h>
using namespace std;
int main(int argc, char *argv[]) {
	try {

		//注入点

		//初始化域参数工厂
		DDS::DomainParticipantFactory_var dpf =
			TheParticipantFactoryWithArgs(argc, argv);
		//创建域参数
		DDS::DomainParticipant_var participant =
			dpf->create_participant(42, // domain ID
				PARTICIPANT_QOS_DEFAULT,
				0,  // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//域参数创建异常处理
		if (!participant) {
			std::cerr << "create_participant failed." << std::endl;
			return 1;
		}
		//注册YH03::Observe类型的类型支持类
		YH03::ObserveTypeSupport_var ts = new YH03::ObserveTypeSupportImpl;
		//注册异常处理
		if (DDS::RETCODE_OK != ts->register_type(participant, "")) {
			std::cerr << "register_type failed." << std::endl;
			return 1;
		}
		//创建主题（Observe YH03）
		CORBA::String_var type_name = ts->get_type_name();
		DDS::Topic_var topic =
			participant->create_topic("Observe YH03",
				type_name,
				TOPIC_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//主题创建异常处理
		if (!topic) {
			std::cerr << "create_topic failed." << std::endl;
			return 1;
		}
		//创建发布者
		DDS::Publisher_var pub =
			participant->create_publisher(PUBLISHER_QOS_DEFAULT,
				0,    // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//发布者创建异常处理
		if (!pub) {
			std::cerr << "create_publisher failed." << std::endl;
			return 1;
		}
		YH03::Observe Observe;
		//数据写者2
		DDS::DataWriter_var writer2 =
			pub->create_datawriter(topic,
				DATAWRITER_QOS_DEFAULT,
				0,    // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//数据写者创建异常
		if (!writer2) {
			std::cerr << "create_datawriter failed." << std::endl;
			return 1;
		}
		//类型窄化
		YH03::ObserveDataWriter_var Observe_writer2 = YH03::ObserveDataWriter::_narrow(writer2);
		//等待集设定
		/*DDS::StatusCondition_var condition2 = writer2->get_statuscondition();
		condition2->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);
		DDS::WaitSet_var ws2 = new DDS::WaitSet;
		ws2->attach_condition(condition2);
		while (true)
		{
			DDS::PublicationMatchedStatus matches2;
			if (writer2->get_publication_matched_status(matches2) != ::DDS::RETCODE_OK)
			{
				std::cerr << "get_publication_matched_status failed." << std::endl;
				return 1;
			}
			//等待集中有实例
			if (matches2.current_count >= 1)
			{
				break;
			}
			DDS::ConditionSeq conditions2;
			//等待时间为60秒
			DDS::Duration_t timeout2 = { 60, 0 };
			if (ws2->wait(conditions2, timeout2) != DDS::RETCODE_OK)
			{
				std::cerr << "wait failed" << std::endl;
				return 1;
			}
		}
		ws2->detach_condition(condition2);*/
		//写入数据样本
		Observe.YH03_Observe_ID = 0;
		Observe.YH03_Observe_name = "观测站壹";
		Observe.YH03_Observe_pm2 = 10;
		Observe.ClockTime = 0;
		Observe.LocalTime = "";
		Observe.Yh03_Observe_tem = 20;
		std::cout << "[S05]开始观测" << std::endl;
		for (int i = 0; i < 50; ++i) {
			time_t nowTime;
			time(&nowTime);
			struct timeb tb;
			ftime(&tb);
			CORBA::String_var s_Time = CORBA::string_dup(ctime(&nowTime));
			Observe.LocalTime = s_Time;
			Observe.ClockTime = clock();
			DDS::ReturnCode_t error = Observe_writer2->write(Observe, DDS::HANDLE_NIL);
			++Observe.YH03_Observe_ID;
			std::cout << "观测站数据编号：  " << Observe.YH03_Observe_ID << std::endl;
			std::cout << "观测站名称：  " << Observe.YH03_Observe_name << std::endl;
			std::cout << "该处PM2.2值：  " << Observe.YH03_Observe_pm2 << std::endl;
			std::cout << "该处温度：  " << Observe.Yh03_Observe_tem << "度" << std::endl;
			Observe.YH03_Observe_pm2 = 10 + 800 * rand() / (RAND_MAX + 1.0);
			Observe.Yh03_Observe_tem = 0 - 30 + 75 * rand() / (RAND_MAX + 1.0);
			if (error != DDS::RETCODE_OK) {
				std::cerr << "write failed." << std::endl;
				return 1;
			}
			Sleep(1800);
		}
		//等待样本被接收确认
		DDS::Duration_t timeout2 = { 30, 0 };
		if (writer2->wait_for_acknowledgments(timeout2) != DDS::RETCODE_OK)
		{
			std::cerr << "wait_for_acknowledgments failed." << std::endl;
			return 1;
		}
		//实例清理
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