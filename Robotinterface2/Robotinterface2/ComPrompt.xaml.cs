using System;
using System.IO.Ports;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort;
using System.Windows.Threading;

namespace Robotinterface2
{
    /// <summary>
    /// Logique d'interaction pour Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private string[] ListAllSerialOpen()
        {
            return SerialPort.GetPortNames();
        }

        private void comboBox_GotFocus(object sender, RoutedEventArgs e)
        {
            string[] menuPortContent = ListAllSerialOpen();

            if (!comboBox.Items.Contains(menuPortContent[0]))
                for (int i = 0; i < menuPortContent.Length; i++)
                {
                    comboBox.Items.Add(menuPortContent[i]);
                }
        }

        private void validCom_Click(object sender, RoutedEventArgs e)
        {
        }

        private void comboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
