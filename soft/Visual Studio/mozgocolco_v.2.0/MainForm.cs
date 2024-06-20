using System;
using System.IO;
using System.IO.Ports;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;
using System.Net;
using System.Resources;
using System.Reflection;
using System.Media;

public enum BaudRate
{
    [Description("2400")]
    Baud2400 = 0,
    [Description("4800")]
    Baud4800 = 1,
    [Description("9600")]
    Baud9600 = 2,
    [Description("14400")]
    Baud14400 = 3,
    [Description("19200")]
    Baud19200 = 4,
    [Description("38400")]
    Baud38400 = 6,
    [Description("57600")]
    Baud57600 = 7,
    [Description("115200")]
    Baud115200 = 8
}

public enum Parity
{
    [Description("Отсутствует")]
    None = 0,
    [Description("Чет")]
    Even = 1,
    [Description("Нечет")]
    Odd = 2
}

public enum DataBits
{
    [Description("7")]
    Seven = 0,
    [Description("8")]
    Eight = 1
}

public enum StopBits
{
    [Description("1")]
    One = 0,
    [Description("2")]
    Two = 2
}

public enum ConverterType
{
    Auto = 1,
    SemiAuto = 0
}

public enum ConnectionError
{
    Ok = 0,

    InvalidArgument = -1,
    PortNotOpened = -2,
    PortError = -5,
    Io = -100,
    Format = -101,
    Timeout = -102,
    InvalidCrc = -103,
    NErr = -104,
    Exception = -105,
    InvalidPacket = -106,
    SensorFailure = -300
}

namespace mozgocolco
{
    public partial class MainForm : Form
    {
        private gameForm gameForm1 = new gameForm();

        public MainForm()
        {
            InitializeComponent();

            gameForm1.MainForm = this;
            gameForm1.Show();
        }

        private void Form1_Load_1(object sender, EventArgs e)
        {
            byte i = 0;

            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
            comboBox1.SelectedIndex = 0;                                            //Выбор COM                    
            serialPort1.PortName = comboBox1.Text;
            comboBox3.SelectedIndex = 3;                           			        //Выбор Bit=8
            serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);
            comboBox2.SelectedIndex = 6;                                            //Выбор BaudRate
            serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);



            gameForm1.KeyPreview = true;

            gameForm1.main_time = (int)numericUpDown1.Value;
            gameForm1.dop_time = (int)numericUpDown2.Value;

            

            while (i < 10)
            {
                dataGridView1.Rows.Add();
                i++;
            }

            i = 1;

            while (i <= 10)
            {
                dataGridView1.Rows[i - 1].Cells[0].Value = i;
                dataGridView1.Rows[i - 1].Cells[1].Value = i + 10;
                i++;
            }

            for (i = 0; i < 10; i++)
            {
                gameForm1.commands[i] = Convert.ToString(dataGridView1.Rows[i].Cells[0].Value);
            }
            for (i = 10; i < 20; i++)
            {
                gameForm1.commands[i] = Convert.ToString(dataGridView1.Rows[i - 10].Cells[1].Value);
            }

            if (Directory.Exists("sounds"))
            {
                string[] soundfiles = Directory.GetFiles("sounds");

                foreach (string filename in soundfiles)
                {
                    comboBox5.Items.Add(filename);
                    comboBox6.Items.Add(filename);
                    comboBox7.Items.Add(filename);
                    comboBox8.Items.Add(filename);
                    comboBox9.Items.Add(filename);
                }

                comboBox4.SelectedIndex = 1;
                comboBox5.SelectedIndex = 0;
                comboBox6.SelectedIndex = 1;
                comboBox7.SelectedIndex = 2;
                comboBox8.SelectedIndex = 3;
                comboBox9.SelectedIndex = 4;
            }
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            Invoke(new EventHandler(DoUpdate));
        }
        private void DoUpdate(object s, EventArgs e)
        {
            int kol_bytes = serialPort1.BytesToRead;  //количество вновь принятых байтов
            byte[] input_bytes = new byte[kol_bytes];
            serialPort1.Read(input_bytes, 0, kol_bytes);

            string s_input = "";

            for (int i = 0; i < kol_bytes; i++)
            {
                s_input += Convert.ToChar(input_bytes[i]);
            }

            textBox2.Text += s_input + "\r\n";
            gameForm1.input_string = s_input;
        }

        private void Scroll_text(object sender, EventArgs e)
        {
            textBox2.SelectionStart = textBox2.Text.Length;            
            textBox2.ScrollToCaret();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());    //Создание списка COM портов 
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)                //если порт открыт
            {
                try
                {
                    serialPort1.Open();
                    button2.Text = "Закрыть";
                    gameForm1.is_serial_port_open = true;
                }
                catch
                {
                    MessageBox.Show("       Ошибка подключения порта" + "\r\n" +
                                    "                             " + comboBox1.Text + "\r\n" +
                                    "(Порт занят или прибор не включен)");
                }

            }
            else
            {
                serialPort1.Close();
                button2.Text = "Открыть";
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.PortName = comboBox1.Text;                              //выбор порта для подключения
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);             //выбор скорости обмена
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);             //выбор количества бит
        }

        private void button8_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(textBox1.Text); //отправляем команду в порт
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            textBox2.Clear();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
                gameForm1.Show();
            else
                gameForm1.Hide();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //sp.Play();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            gameForm1.sound_answ_path = Convert.ToString(comboBox5.SelectedItem);
            gameForm1.sound_fals_path = Convert.ToString(comboBox6.SelectedItem);
            gameForm1.sound_stop_path = Convert.ToString(comboBox7.SelectedItem);
            gameForm1.sound_strt_path = Convert.ToString(comboBox9.SelectedItem);
            gameForm1.sound_time_path = Convert.ToString(comboBox8.SelectedItem);

            gameForm1.refresh_flag = true;
            
        }

        private void label10_Click(object sender, EventArgs e)
        {

        }

        private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBox6_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            gameForm1.main_time = (int)numericUpDown1.Value;
            gameForm1.dop_time = (int)numericUpDown2.Value;
            gameForm1.mode = comboBox4.SelectedIndex;
            gameForm1.num_of_comands = (byte)numericUpDown3.Value;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            for (byte i = 0; i < 10; i++)
            {
                gameForm1.commands[i] = Convert.ToString(dataGridView1.Rows[i].Cells[0].Value);
            }
            for (byte i = 10; i < 20; i++)
            {
                gameForm1.commands[i] = Convert.ToString(dataGridView1.Rows[i - 10].Cells[1].Value);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (gameForm1.send_to_port < 255)
            {
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write(Convert.ToString(gameForm1.send_to_port)); //отправляем команду в порт
                }
                gameForm1.send_to_port = 255;
            }
        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            if (Directory.Exists("sounds"))
            {
                string[] soundfiles = Directory.GetFiles("sounds");

                foreach (string filename in soundfiles)
                {
                    comboBox5.Items.Add(filename);
                    comboBox6.Items.Add(filename);
                    comboBox7.Items.Add(filename);
                    comboBox8.Items.Add(filename);
                    comboBox9.Items.Add(filename);
                }
                comboBox5.SelectedIndex = 0;
                comboBox6.SelectedIndex = 1;
                comboBox7.SelectedIndex = 2;
                comboBox8.SelectedIndex = 3;
                comboBox9.SelectedIndex = 4;
            }
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {

        }
    }
}
