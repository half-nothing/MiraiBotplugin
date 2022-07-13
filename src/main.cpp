#include <iostream>
#include <vector>
#include <mirai.h>
#include "nlohmann/json.hpp"
using namespace std;
using namespace Cyan;

//函数声明
inline string transform(json);

//函数定义
inline string transform(json message)
{
    string output;
    for(auto & i : message)
    {
        string type = i["type"];
        if(type == "Image")
        {
            output += "[图片]";
        }else if(type == "Plain")
        {
            output += i["text"];
        }else if(type == "At")
        {
            output += "@";
            int targetQQ = i["target"];
            output += to_string(targetQQ);
        }
    }
    return output;
}

//重载一个忽略图片的transform函数
inline string transform(json message, bool textOnly)
{
    string output;
    for(auto & i : message)
    {
        string type = i["type"];
        if(type == "Plain")
        {
            output += i["text"];
        }else if(type == "At")
        {
            output += "@";
            int targetQQ = i["target"];
            output += to_string(targetQQ);
        }
    }
    return output;
}

int main()
{
#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
#endif

    //初始化bot
	MiraiBot bot;
	SessionOptions opts = SessionOptions::FromJsonFile("./config.json");

	while (true)
	{
		try
		{
			cout << "尝试与 mirai-api-http 建立连接..." << endl;
			bot.Connect(opts);
			break;
		}
		catch (const std::exception& error)
		{
			cout << error.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "成功连接到Mirai" << endl;
    cout << "插件正常运行中，请先输入\"exit\"再退出" << endl;

    //捕获群消息，记录日志
    bot.On<GroupMessage>([&](const GroupMessage& event)
    {
        cout << "[收到消息] <- [" + event.Sender.Group.Name + "] " << event.Sender.MemberName << "(" << event.Sender.QQ << "): " + transform(event.MessageChain.ToJson()) << endl;
    });

    bot.On<GroupMessage>([&](const GroupMessage& event)
    {
        string message = transform(event.MessageChain.ToJson(), true);
    });

	// 在失去与mah的连接后重连
	bot.On<LostConnection>([&](const LostConnection& event)
    {
        cout << event.ErrorMessage << " (" << event.Code << ")" << endl;
        while (true)
        {
            try
            {
                cout << "尝试连接 mirai-api-http..." << endl;
                bot.Reconnect();
                cout << "与 mirai-api-http 重新建立连接!" << endl;
                break;
            }
            catch (const std::exception& error)
            {
                cout << error.what() << endl;
            }
            MiraiBot::SleepSeconds(1);
        }
    });

	string cmd;
	while (cin >> cmd)
	{
		if (cmd == "exit")
		{
			// 程序结束前必须调用 Disconnect，否则 mirai-api-http 会内存泄漏。
			bot.Disconnect();
			break;
		}
	}

	return 0;
}