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
using System.Security.RightsManagement;
using MouseKeyboardActivityMonitor.WinApi;
using MouseKeyboardActivityMonitor;
using System.Windows.Forms;
using System.Text;
using System.IO;
using Utilities;

namespace Robotinterface2
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>

    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        DispatcherTimer timerAffichageData;
        DispatcherTimer stateMachine;
        DispatcherTimer timestampTimer;

        private string receivedtext;
        private string receivedtextAnt;
        private string settingsPath = @"settings.txt";
        private string logPath = @"log.txt";

        Queue<byte> byteListReceived = new Queue<byte>();

        bool isPortSelected = false;

        int autoPortSetting = 0;

        long receptionTimeOdo = 0;
        float positionXOdo = 0;
        float positionYOdo = 0;
        float angleOdo = 0;
        float linearSpeedOdo = 0;
        float angularSpeedOdo = 0;

        int TelemetreEG = 0;
        int TelemetreG = 0;
        int TelemetreC = 0;
        int TelemetreD = 0;
        int TelemetreED = 0;

        int distanceECotes = 20;
        int distanceECotesEloigne = 3;
        int distanceCotes = 22;
        int distanceCotesEloigne = 35;
        int distanceCentre = 23;
        int distanceCentreEloigne = 40;
        int distanceSecurite = 18;

        int coteDemiTour = 0;
        int coteChoisi = 0;
        int nombreTours = 0;

        long timestamp = 0;
        long timestamp_1 = 0;

        int robotState = 0;
        long receptionTimeState = 0;
        int stateChanged = -1;

        bool autoControlActivated = true;
        bool stateMachineActivated = true;
        int controlMode = 0;

        private readonly KeyboardHookListener m_KeyboardHookManager;

        public struct Message
        {
            public int msgDecodedFunction;
            public int msgDecodedPayloadLength;
            public byte[] msgDecodedPayload;
            public bool isValidChecksum;
        }

        Queue<Message> Messages = new Queue<Message>();

        public enum stateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15,
            STATE_AVANCE_LENTEMENT = 16,
            STATE_AVANCE_LENTEMENT_EN_COURS = 17,
            STATE_TOURNE_LENTEMENT_DROITE = 18,
            STATE_TOURNE_LENTEMENT_DROITE_EN_COURS = 19,
            STATE_TOURNE_LENTEMENT_GAUCHE = 20,
            STATE_TOURNE_LENTEMENT_GAUCHE_EN_COURS = 21,
            STATE_JACQUES = 100
        }

        private enum obstacle
        {
            PAS_D_OBSTACLE,
            OBSTACLE_A_GAUCHE,
            OBSTACLE_A_DROITE,
            OBSTACLE_EN_FACE,
            OBSTACLE_ELOIGNE,
            OBSTACLE_CENTRE_DROIT,
            OBSTACLE_CENTRE_GAUCHE,
            OBSTACLE_E_DROIT,
            OBSTACLE_E_GAUCHE,
            OBSTACLE_CRITIQUE_DROITE,
            OBSTACLE_CRITIQUE_GAUCHE
        }

        public MainWindow()
        {
            InitializeComponent();
            /*-----------------------------------Init Timer-------------------------*/
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 10);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
            /*-----------------------------------Init Timer-------------------------*/
            timerAffichageData = new DispatcherTimer();
            timerAffichageData.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichageData.Tick += TimerAffichageData_Tick;
            timerAffichageData.Start();
            /*-----------------------------------Init Timer-------------------------*/
            stateMachine = new DispatcherTimer();
            stateMachine.Interval = new TimeSpan(0, 0, 0, 0, 5);
            stateMachine.Tick += stateMachine_Tick;
            stateMachine.Start();
            /*-----------------------------------Init Timer-------------------------*/
            timestampTimer = new DispatcherTimer();
            timestampTimer.Interval = new TimeSpan(0, 0, 0, 0, 50);
            timestampTimer.Tick += timeStampTimer_Tick;
            timestampTimer.Start();
            /*----------------------------------------------------------------------*/
            m_KeyboardHookManager = new KeyboardHookListener(new GlobalHooker());
            m_KeyboardHookManager.Enabled = true;
            m_KeyboardHookManager.KeyDown += HookManager_KeyDown;


            using (FileStream fs = File.Open(settingsPath, FileMode.Open, FileAccess.Read))
            {
                byte[] b = new byte[1024];
                UTF8Encoding temp = new UTF8Encoding(true);
                string settingsString = "";

                while (fs.Read(b, 0, b.Length) > 0)
                {
                    settingsString += temp.GetString(b);
                }

                int j;
                int n=0;
                for (int i = 0; i < settingsString.Length; i++)
                {
                    if (settingsString[i] == '\n')
                    {
                        switch (n)
                        {
                            case 0:
                                for (j = 0; settingsString[i - j] != ' '; j++) ;
                                for (int k = 0; k < j - 2; k++)
                                    autoPortSetting += ((char)(settingsString[i - j + k + 1]) - 48) * (int)Math.Pow(10, (j + k - 3));
                                break;

                            case 1:
                                //other settings
                                break;
                        }
                        n++;
                    }
                }

            }

            //System.IO.File.WriteAllLines(logPath, new string[] { });
        }   /*----------------------------------------------------------------------*/

        private void HookManager_KeyDown (object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (!autoControlActivated)
{
                switch (e.KeyCode)
                {
                    case Keys.Left:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                            (byte) stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE});
                        stateChanged = (int) stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE;
                        break;

                    case Keys.Right:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                            (byte) stateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                        stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE;
                        break;

                    case Keys.Up:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                            (byte) stateRobot.STATE_AVANCE });
                        stateChanged = (int)stateRobot.STATE_AVANCE;
                        break;

                    case Keys.Down:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                            (byte) stateRobot.STATE_ARRET });
                        stateChanged = (int)stateRobot.STATE_ARRET;
                        break;

                    case Keys.PageDown:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                            (byte) stateRobot.STATE_RECULE });
                        stateChanged = (int)stateRobot.STATE_RECULE;
                        break;
                }
            }
        }

    private void TimerAffichageData_Tick(object sender, EventArgs e)
        {
            textBoxData.Clear();
            textBoxData.Text += "E=";
            textBoxData.Text += TelemetreEG;
            textBoxData.Text += "cm  \nG=";
            textBoxData.Text += TelemetreG;
            textBoxData.Text += "cm  \nC=";
            textBoxData.Text += TelemetreC;
            textBoxData.Text += "cm  \nD=";
            textBoxData.Text += TelemetreD;
            textBoxData.Text += "cm  \nE=";
            textBoxData.Text += TelemetreED;
            textBoxData.Text += "cm\n\n";
            textBoxData.Text += "[ETAPE";
            if (isValidChecksum == true)
                textBoxData.Text += "]";
            else
                textBoxData.Text += ", invalid checksum !] ";
            textBoxData.Text += "\n" + ((stateRobot)(robotState)).ToString() + "\n(" + receptionTimeState + "ms)\n\n";
            textBoxData.Text += "x=" + positionXOdo + " y=" + positionYOdo + " angle=" + angleOdo + " v=" + linearSpeedOdo + " va=" + angularSpeedOdo;
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //caca
            //using (System.IO.StreamWriter lf = new System.IO.StreamWriter(logPath, true))
            //{
            //    lf.WriteLine(TelemetreEG + "  " + TelemetreG + "  " + TelemetreC + "  " + TelemetreD + "  " + TelemetreD + " - " + robotState + "  " + stateChanged + "  @" + timestamp);
            //}

            if (autoPortSetting == 1 && isPortSelected == false)
            {
                string[] menuPortContentAS = ListAllSerialOpen();
                serialPort1 = new ReliableSerialPort(menuPortContentAS[menuPortContentAS.Length - 1], 115200, Parity.None, 8, StopBits.One); //entrée USB à droite
                serialPort1.DataReceived += SerialPort1_DataReceived;
                serialPort1.Open();
                isPortSelected = true;
                UartEncodeAndSendMessage(0x0052, 1, new byte[] { 0 });
                autoControlActivated = false;
                stateMachineActivated = true;
                UartEncodeAndSendMessage(0x0020, 2, new byte[] { 1, 1 });
            }

            if ((stateRobot)(robotState) != (stateRobot)(stateChanged) &&
                (stateRobot)(robotState) != (stateRobot)(stateChanged)+1)
                UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte) stateChanged });
            
            while (Messages.Count>0)
            {
                Message CurMsg = Messages.Dequeue();
                switch (CurMsg.msgDecodedFunction)
                {
                    case 0x0080: // transmission texte
                        textBoxReception.Text += "[Text (";
                        textBoxReception.Text += CurMsg.msgDecodedPayloadLength;
                        textBoxReception.Text += ")";
                        if (CurMsg.isValidChecksum == true)
                            textBoxReception.Text += "] ";
                        else
                            textBoxReception.Text += ", invalid checksum !] ";
                        for (int i = 0; i < CurMsg.msgDecodedPayloadLength; i++)
                        {
                            textBoxReception.Text += (char)CurMsg.msgDecodedPayload[i];
                        }
                        textBoxReception.Text += "\n";
                        break;

                    case 0x0020: // reglage LED
                        textBoxReception.Text += "[LED";
                        if (CurMsg.isValidChecksum == false)
                            textBoxReception.Text += " invalid checksum !] ";
                        else
                            textBoxReception.Text += "] ";
                        textBoxReception.Text += "LED ";
                        textBoxReception.Text += CurMsg.msgDecodedPayload[0];
                        if (CurMsg.msgDecodedPayload[1] == 1)
                            textBoxReception.Text += " ON";
                        else
                            textBoxReception.Text += " OFF";
                        textBoxReception.Text += "\n";
                        break;

                    case 0x0030: // distance télémètre IR
                        if(CurMsg.isValidChecksum == true)
                        {
                            TelemetreG = CurMsg.msgDecodedPayload[0];
                            TelemetreC = CurMsg.msgDecodedPayload[1];
                            TelemetreD = CurMsg.msgDecodedPayload[2];
                            if (CurMsg.msgDecodedPayloadLength == 5)
                            {
                                TelemetreEG = CurMsg.msgDecodedPayload[3];
                                TelemetreED = CurMsg.msgDecodedPayload[4];
                            }
                        }
                        else
                        {
                            textBoxReception.Text += "[IR invalid checksum]\n";
                        }
                        break;

                    case 0x0040: // Consigne vitesse
                        textBoxReception.Text += "[CONSIGNE";
                        if (CurMsg.isValidChecksum == true)
                            textBoxReception.Text += "] ";
                        else
                            textBoxReception.Text += ", invalid checksum !] ";
                        textBoxReception.Text += "G=";
                        textBoxReception.Text += (float)CurMsg.msgDecodedPayload[0]*100.0/255.0;
                        textBoxReception.Text += "%  D=";
                        textBoxReception.Text += (float)CurMsg.msgDecodedPayload[1]*100.0/255.0;
                        textBoxReception.Text += "%\n";
                        break;

                    case 0x0050: // Etape en cours
                        receptionTimeState = (((int)CurMsg.msgDecodedPayload[1]) << 24) +
                                        (((int)CurMsg.msgDecodedPayload[2]) << 16) +
                                        (((int)CurMsg.msgDecodedPayload[3]) << 8) +
                                        (((int)CurMsg.msgDecodedPayload[4]) << 0);
                        receptionTimeState *= 5;
                        robotState = CurMsg.msgDecodedPayload[0];
                        break;

                    case 0x0061: // odométrie
                        byte[] tab = new byte[4];

                        // timestamp
                        tab = CurMsg.msgDecodedPayload.GetRange(0, 4);
                        receptionTimeOdo = tab[0] << 24;
                        receptionTimeOdo += tab[0] << 16;
                        receptionTimeOdo += tab[0] << 8;
                        receptionTimeOdo += tab[0] << 0;

                        // odo 1
                        tab = CurMsg.msgDecodedPayload.GetRange(4, 4);
                        positionXOdo = BitConverter.ToInt32(tab, 0);
                        //positionXOdo = tab.GetFloat();

                        // odo 2
                        tab = CurMsg.msgDecodedPayload.GetRange(8, 4);
                        positionYOdo = tab.GetFloat();

                        // angle
                        tab = CurMsg.msgDecodedPayload.GetRange(12, 4);
                        angleOdo = tab.GetFloat();

                        // linear speed
                        tab = CurMsg.msgDecodedPayload.GetRange(16, 4);
                        linearSpeedOdo = tab.GetFloat();

                        // angular speed
                        tab = CurMsg.msgDecodedPayload.GetRange(20, 4);
                        angularSpeedOdo = tab.GetFloat();
                        break;
                }
            }

                //textBoxReception.Text += "0x" + byteListReceived.Dequeue().ToString("X2") + " "
                        //if (receivedtext != receivedtextAnt)
            //{
            //    textBoxReception.Text = receivedtext;
            //    receivedtextAnt = receivedtext;
            //}
        }

        private void timeStampTimer_Tick(object sender, EventArgs e)
        {
            if ((stateRobot)(robotState) == stateRobot.STATE_JACQUES)
            {
                timestamp = 0;
                timestamp_1 = 0;
            }
            else
            {
                timestamp++;
                timestamp_1++;
            }
        }

        private void stateMachine_Tick(object sender, EventArgs e)
        {
            obstacle positionObstacle = obstacle.PAS_D_OBSTACLE;

            if (stateMachineActivated)
            {
                //Détermination de la position des obstacles en fonction des télémètres
                if (TelemetreC < distanceCentre ||
                        ((TelemetreG < distanceSecurite || TelemetreEG < distanceSecurite) &&
                        (TelemetreD < distanceSecurite || TelemetreED < distanceSecurite)))
                    positionObstacle = obstacle.OBSTACLE_EN_FACE;
                else if (TelemetreD < distanceCotes)
                    //            (TelemetreD > 500 && 
                    //            TelemetreED < distanceSecurite && TelemetreC < distanceSecurite)) //Obstacle à droite
                    positionObstacle = obstacle.OBSTACLE_CRITIQUE_DROITE;
                else if (TelemetreG < distanceCotes)
                    positionObstacle = obstacle.OBSTACLE_CRITIQUE_GAUCHE;
                else if (TelemetreD < distanceCotes &&
                        TelemetreED < distanceECotes)
                    positionObstacle = obstacle.OBSTACLE_A_DROITE;
                else if (TelemetreG < distanceCotes &&
                        TelemetreEG < distanceECotes)
                    positionObstacle = obstacle.OBSTACLE_A_GAUCHE;
                else if (TelemetreG > distanceCotes &&
                        TelemetreC > distanceCentre &&
                        TelemetreEG < distanceECotes)
                    positionObstacle = obstacle.OBSTACLE_E_GAUCHE;
                else if (TelemetreD > distanceCotes &&
                        TelemetreC > distanceCentre &&
                        TelemetreED < distanceECotes)
                    positionObstacle = obstacle.OBSTACLE_E_DROIT;
                else if (TelemetreC < distanceCentreEloigne ||
                        TelemetreD < distanceCotesEloigne ||
                        TelemetreG < distanceCotesEloigne ||
                        TelemetreED < distanceECotesEloigne ||
                        TelemetreEG < distanceECotesEloigne)
                    positionObstacle = obstacle.OBSTACLE_ELOIGNE;
                else if (TelemetreD > distanceCotes &&
                        TelemetreC > distanceCentre &&
                        TelemetreG > distanceCotes &&
                        TelemetreEG > distanceECotes &&
                        TelemetreED > distanceECotes) //pas d?obstacle
                    positionObstacle = obstacle.PAS_D_OBSTACLE;

                //Détermination de l?état à venir du robot
                if (positionObstacle == obstacle.PAS_D_OBSTACLE)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_AVANCE });
                    stateChanged = (int)stateRobot.STATE_AVANCE;
                    if (timestamp_1 - timestamp > 20)
                    {
                        coteChoisi = 0;
                        nombreTours = 0;
                    }
                }
                else if (positionObstacle == obstacle.OBSTACLE_EN_FACE || nombreTours > 20)
                {
                    timestamp_1 = timestamp;
                    if (Math.Abs(TelemetreG - TelemetreD) > 1 && coteChoisi == 0)
                    {
                        if (TelemetreG > TelemetreD)
                        {
                            UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                            stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE;
                            coteChoisi = 1;
                        }
                        else
                        {
                            UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                            stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE;
                            coteChoisi = 2;
                        }
                    }
                    else
                    {
                        if (coteChoisi != 0)
                        {
                            UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)(6 + coteChoisi * 2) });
                            stateChanged = 6 + coteChoisi * 2;
                        }
                    }
                }
                else if (positionObstacle == obstacle.OBSTACLE_A_DROITE)
                {
                    nombreTours++;
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_GAUCHE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_GAUCHE;
                }
                else if (positionObstacle == obstacle.OBSTACLE_A_GAUCHE)
                {
                    nombreTours++;
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_DROITE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_DROITE;
                }
                else if (positionObstacle == obstacle.OBSTACLE_ELOIGNE)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_AVANCE_LENTEMENT });
                    stateChanged = (int)stateRobot.STATE_AVANCE_LENTEMENT;
                    if (timestamp_1 - timestamp > 20)
                    {
                        coteChoisi = 0;
                        nombreTours = 0;
                    }
                }
                else if (positionObstacle == obstacle.OBSTACLE_CENTRE_DROIT)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE;
                }
                else if (positionObstacle == obstacle.OBSTACLE_CENTRE_GAUCHE)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE;
                }
                else if (positionObstacle == obstacle.OBSTACLE_E_DROIT)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_LENTEMENT_GAUCHE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_LENTEMENT_GAUCHE;
                    nombreTours++;
                }
                else if (positionObstacle == obstacle.OBSTACLE_E_GAUCHE)
                {
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_LENTEMENT_DROITE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_LENTEMENT_DROITE;
                    nombreTours++;
                }
                else if (positionObstacle == obstacle.OBSTACLE_CRITIQUE_DROITE)
                {
                    nombreTours++;
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE;
                }
                else if (positionObstacle == obstacle.OBSTACLE_CRITIQUE_GAUCHE)
                {
                    nombreTours++;
                    UartEncodeAndSendMessage(0x0051, 1, new byte[] { (byte)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                    stateChanged = (int)stateRobot.STATE_TOURNE_SUR_PLACE_DROITE;
                }
            }
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //receivedtext += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (int i=0; i<e.Data.Length; i++)
            {
                DecodeMessage(e.Data[i]);
            }
        }
        
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            switch (comboBox.SelectedItem)
            {
                case "Text":
                    byte[] arrayT = Encoding.ASCII.GetBytes(textBoxEmission.Text);
                    UartEncodeAndSendMessage(0x0080, arrayT.Length, arrayT);
                    break;

                case "Led":
                    byte[] arrayL = Encoding.ASCII.GetBytes(textBoxEmission.Text + 2);
                    arrayL[0] -= 48; arrayL[1] -= 48;
                    UartEncodeAndSendMessage(0x0020, 2, arrayL);
                    break;

                case "Speed":
                    
                    break;

                case "Single byte":
                    Sendmessage();
                    break;
            }

            textBoxEmission.Clear();
        }

        private void textBoxEmission_KeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                if (textBoxEmission.Text.Length > 2)
                {
                    int i;

                    for (i = textBoxEmission.Text.Length - 1;
                        textBoxEmission.Text.ToCharArray()[i] == 0x0A ||
                        textBoxEmission.Text.ToCharArray()[i] == 0x0D;
                        i--) ;

                    textBoxEmission.Text = textBoxEmission.Text.Remove(i + 1, textBoxEmission.Text.Length - i - 1);

                    buttonEnvoyer_Click(sender, e);
                }
                else
                    textBoxEmission.Clear();
            }
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Clear();
            receivedtext = "";
        }




        void Sendmessage()
        {
            string texteE = textBoxEmission.Text;
            serialPort1.WriteLine(texteE);
            textBoxEmission.Clear();
        }

        private void buttonTEST_Click(object sender, RoutedEventArgs e)
        {
            if (!isPortSelected)
                return;

            //byte[] byteList = new byte[20];

            //for (int i=0; i<20; i++)
            //{
            //    byteList[i] = (byte)(2 * i);
            //}
            //serialPort1.Write(byteList, 0, 20

            //Encoding.ASCII.GetBytes("bonjour");

            //for (int i = 0; i < array.Length; i++)
            //{
            //    textBoxData.Text += (char) array[i];
            //}
            //textBoxData.Text += "  ";
            byte[] array = Encoding.ASCII.GetBytes(textBoxEmission.Text);
            array[0] -= 48;

            UartEncodeAndSendMessage(0x0051, 1, array);

            textBoxEmission.Clear();
        }

        byte CalculateCheckSum (int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte actuel, sortie = 0;

            sortie ^= 0xFE;
            sortie ^= (byte)((msgFunction & 0xFF00) / 0x100);
            sortie ^= (byte)(msgFunction & 0xFF);
            sortie ^= (byte)((msgPayloadLength & 0xFF00) / 0x100);
            sortie ^= (byte)(msgPayloadLength & 0xFF);

            for (int i=0; i<msgPayloadLength; i++)
            {
                actuel = msgPayload[i];
                sortie ^= actuel;
            }

            return sortie;
        }

        void UartEncodeAndSendMessage (int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] sortie = new byte[msgPayloadLength + 6];

            sortie[0] = 0xFE;

            sortie[1] = (byte)((msgFunction & 0xFF00) / 0x100);
            sortie[2] = (byte)(msgFunction & 0xFF);

            sortie[3] = (byte)((msgPayloadLength & 0xFF00) / 0x100);
            sortie[4] = (byte)(msgPayloadLength & 0xFF);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                sortie[i + 5] = msgPayload[i];
            }

            sortie[msgPayloadLength + 5] = CalculateCheckSum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(sortie, 0, msgPayloadLength + 6);
        }

        private string[] ListAllSerialOpen()
        {
            return SerialPort.GetPortNames();
        }

        private void comboBox_GotFocus(object sender, RoutedEventArgs e)
        {
            if (isPortSelected == false)
            {
                string[] menuPortContent = ListAllSerialOpen();

                if (!comboBox.Items.Contains(menuPortContent[menuPortContent.Length - 1]))
                {
                    comboBox.Items.Clear();
                    for (int i = 0; i < menuPortContent.Length; i++)
                    {
                        comboBox.Items.Add(menuPortContent[i]);
                    }
                }
            }
            else
            {
                
            }
        }

        private void comboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (isPortSelected == false)
            {
                if (serialPort1 == null && comboBox.SelectedItem != null && comboBox.Text != null)
                {
                    serialPort1 = new ReliableSerialPort(comboBox.SelectedItem.ToString(), 115200, Parity.None, 8, StopBits.One); //entrée USB à droite
                    serialPort1.DataReceived += SerialPort1_DataReceived;
                    serialPort1.Open();

                    //On change la fonction de la comboBox
                    isPortSelected = true;
                    comboBox.Items.Clear();

                    comboBox.Items.Add("Text");
                    comboBox.Items.Add("Led");
                    comboBox.Items.Add("Speed");
                    comboBox.Items.Add("Single byte");

                    comboBox.SelectedItem = comboBox.Items[0];

                    AutomaticMode_Click(sender, e);
                    //UartEncodeAndSendMessage(0x0052, 1, new byte[] { (byte) 0});
                }
            }
            else
            {
                
            }

            /*----------------------------Init du port série + évenement datareceived------------------------------------*/
            //serialPort1 = new ReliableSerialPort("COM7", 115200, Parity.None, 8, StopBits.One); //entrée USB à droite
            //serialPort1.DataReceived += SerialPort1_DataReceived;
            //serialPort1.Open();
            /*-----------------------------------------------------------------------------------------------------------*/
        }

        //private void menuPort_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        //{
        //    string[] menuPortContent = ListAllSerialOpen();

        //    menuPort.Items.Add(menuPortContent);
        //}

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload = new byte[256];
        int msgDecodedPayloadIndex = 0;
        byte calculatedChecksum, receivedChecksum;
        bool isValidSOF = true, isValidChecksum = true, decodeFinishedFlag = false;

        private void AutomaticMode_Click(object sender, RoutedEventArgs e)
        {
            if (!isPortSelected)
                return;

            // On cahnge de mode à chaque appui
            controlMode++;
            if (controlMode > 2)
                controlMode = 0;
            switch (controlMode)
            {
                case 0:
                    autoControlActivated = true;
                    stateMachineActivated = false;
                    break;

                case 1:
                    autoControlActivated = false;
                    stateMachineActivated = true;
                    break;

                case 2:
                    autoControlActivated = false;
                    stateMachineActivated = false;
                    break;
            }
            //--------------------

            byte[] payload = new byte[1];
            if (autoControlActivated)
            {
                AutomaticMode.Content = "Onboard";
                AutomaticMode.Background = Brushes.Olive;
                payload[0] = 1;
            }
            else if (stateMachineActivated)
            {
                AutomaticMode.Content = "Auto";
                AutomaticMode.Background = Brushes.DarkBlue; 
                payload[0] = 0;
            }
            else
            {
                AutomaticMode.Content = "Manuel";
                AutomaticMode.Background = Brushes.DarkRed;
                payload[0] = 0;
            }
            UartEncodeAndSendMessage(0x0052, 1, payload);
        }

        

        byte lastInvalidChar = 0x00;

        private void DecodeMessage (byte c)
        {
            Message DecMsg;

            switch (rcvState)
            {
                case StateReception.Waiting :
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                        isValidSOF = true;
                    }
                    else
                    {
                        isValidSOF = false;
                        lastInvalidChar = c;
                    }
                    break;

                case StateReception.FunctionMSB :
                    msgDecodedFunction = c*0x100;
                    rcvState = StateReception.FunctionLSB;
                    break;
                    
                case StateReception.FunctionLSB :
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                    
                case StateReception.PayloadLengthMSB :
                    msgDecodedPayloadLength = c * 0x100;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                    
                case StateReception.PayloadLengthLSB :
                    msgDecodedPayloadLength += c;
                    rcvState = StateReception.Payload;
                    break;
                    
                case StateReception.Payload :
                    if (msgDecodedPayloadIndex < msgDecodedPayloadLength-1 && msgDecodedPayloadLength < 256)
                    {
                        msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    }
                    else
                    {
                        msgDecodedPayload[msgDecodedPayloadIndex] = c;//caca
                        rcvState = StateReception.CheckSum;
                        msgDecodedPayloadIndex = 0;
                    }
                    break;
                    
                case StateReception.CheckSum:
                    receivedChecksum = c;
                    if (msgDecodedPayloadLength < 256)
                        calculatedChecksum = CalculateCheckSum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {
                        isValidChecksum = true;// Success ,  on a un message  valide
                    }
                    else
                    {
                        isValidChecksum = false;
                    }
                    decodeFinishedFlag = true;
                    rcvState = StateReception.Waiting;

                    DecMsg.msgDecodedFunction = msgDecodedFunction;
                    DecMsg.msgDecodedPayloadLength = msgDecodedPayloadLength;
                    DecMsg.msgDecodedPayload = msgDecodedPayload;
                    DecMsg.isValidChecksum = isValidChecksum;

                    Messages.Enqueue(DecMsg);
                    break;
                
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }
    }
}