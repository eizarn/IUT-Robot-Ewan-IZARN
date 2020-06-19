using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using WpfRobotInterface;
using System.Windows;

namespace Robot
{
    class Program
    {
        static bool usingRobotInterface = true;
        static RobotInterface interfaceRobot;
        static void Main(string[] args)
        {
            if (usingRobotInterface)
                StartRobotInterface();
        }

        static Thread t1;
        static void StartRobotInterface()
        {
            t1 = new Thread(() =>
            {
                interfaceRobot = new RobotInterface();
                interfaceRobot.ShowDialog();
            });
            t1.SetApartmentState(ApartmentState.STA);
            t1.Start();
        }
    }
}
