using System;
using System.Collections.Generic;
using System.IO.Ports;
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

namespace DeusVultInterface
{
    
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
        
    public partial class MainWindow : Window
    {
        SerialPort serialPort1;

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM12", 115200, Parity.None, 8, StopBits.One);
            serialPort1.Open();
        }

        public void sendMessage()
        {
            serialPort1.WriteLine(textBoxEmission.Text);
            // textBoxReception.Text = textBoxReception.Text + "Reçu : " + textBoxEmission.Text;
            textBoxEmission.Text = "";
        }

        private void textBoxEmission_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            sendMessage();
            textBoxReception.Text = textBoxReception.Text + "\n";
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                sendMessage();
            }
        }
    }
}
