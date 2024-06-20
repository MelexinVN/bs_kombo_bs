using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;

namespace mozgocolco
{
    public partial class gameForm : Form
    {
        public long time_clock = 0;
        public bool visiblity = true;
        public bool is_serial_port_open = false;
        public string input_string = " ";
        public string sound_answ_path = "sounds/answer.wav";
        public string sound_fals_path = "sounds/falstart.wav";
        public string sound_stop_path = "sounds/stop.wav";
        public string sound_strt_path = "sounds/start.wav";
        public string sound_time_path = "sounds/time.wav";
        public byte num_of_comands = 20;
        public byte send_to_port = 255;

        public string[] commands = new string[20];

        public int main_time = 60;
        public int dop_time = 20;
        public int mode = 1;

        public bool refresh_flag = false;

        private MainForm m_owner = null;

        SoundPlayer answ_p = new SoundPlayer();
        SoundPlayer fals_p = new SoundPlayer();
        SoundPlayer stop_p = new SoundPlayer();
        SoundPlayer strt_p = new SoundPlayer();
        SoundPlayer time_p = new SoundPlayer();

        private byte count = 0;

        public MainForm MainForm
        {
            set { m_owner = value; }
        }

        public gameForm()
        {
            InitializeComponent();
        }

        private void gameForm_Load(object sender, EventArgs e)
        {
            answ_p.SoundLocation = sound_answ_path;
            fals_p.SoundLocation = sound_fals_path;
            stop_p.SoundLocation = sound_stop_path;
            strt_p.SoundLocation = sound_strt_path;
            time_p.SoundLocation = sound_time_path;

            answ_p.Load();
            fals_p.Load();
            stop_p.Load();
            strt_p.Load();
            time_p.Load();

            label01.Text = " ";
            label02.Text = " ";
            label03.Text = " ";
            label04.Text = " ";
            label05.Text = " ";
            label06.Text = " ";
            label07.Text = " ";
            label08.Text = " ";
            label09.Text = " ";
            label10.Text = " ";
            label11.Text = " ";
            label12.Text = " ";
            label13.Text = " ";
            label14.Text = " ";
            label15.Text = " ";
            label16.Text = " ";
            label17.Text = " ";
            label18.Text = " ";
            label19.Text = " ";
            label20.Text = " ";

            timer2.Enabled = true;
        }

        private void time1Update(object sender, EventArgs e)
        {
            time_clock++;
            byte mins, secs;
            string s_mins, s_secs;
            
            mins = (byte)(time_clock / 60);
            if (mins >= 10)   s_mins = Convert.ToString(mins);
            else              s_mins = '0' + Convert.ToString(mins);

            secs = (byte)(time_clock - mins * 60);
            if (secs >= 10)   s_secs = Convert.ToString(secs);
            else              s_secs = '0' + Convert.ToString(secs);

            label1.Text = s_mins + ':' + s_secs;

            if (time_clock == dop_time) time_p.Play();

            if (time_clock == main_time)
            {
                stop_p.Play();
                time_clock = 0;
                timer1.Enabled = false;
            }


        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (button2.Text == "Старт")
            {
                start_procedure();
            }
            else 
            {
                stop_procedure();
            }
            
        }

        private void start_procedure()
        {
            strt_p.Play();
            button2.Text = "Стоп";
            if (timer1.Enabled)
            {
                timer1.Enabled = false;
            }
            else
            {
                timer1.Enabled = true;
            }
        }

        private void stop_procedure()
        {
            stop_p.Play();
            button2.Text = "Старт";
            if (timer1.Enabled)
            {
                timer1.Enabled = false;
            }
            else
            {
                timer1.Enabled = true;
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (input_string[0] != ' ')
            {
                if (timer1.Enabled) answ_p.Play();
                else fals_p.Play();

                for (int i = 0; i < input_string.Length; i++)
                {
                    if (input_string[i] == '0')
                    {
                        label01.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[0] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[0] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '1')
                    {
                        label02.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[1] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[1] + "\r\n"; 
                        count++;
                    }
                    if (input_string[i] == '2')
                    {
                        label03.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[2] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[2] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '3')
                    {
                        label04.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[3] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[3] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '4')
                    {
                        label05.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[4] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[4] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '5')
                    {
                        label06.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[5] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[5] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '6')
                    {
                        label07.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[6] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[6] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '7')
                    {
                        label08.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[7] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[7] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '8')
                    {
                        label09.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[8] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[8] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == '9')
                    {
                        label10.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[9] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[9] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'a')
                    {
                        label11.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[10] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[10] + "\r\n";
                        count++;
                    }

                    if (input_string[i] == 'b')
                    {
                        label12.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[11] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[11] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'c')
                    {
                        label13.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[12] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[12] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'd')
                    {
                        label14.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[13] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[13] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'e')
                    {
                        label15.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[14] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[14] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'f')
                    {
                        label16.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[15] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[15] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'g')
                    {
                        label17.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[16] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[16] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'h')
                    {
                        label18.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[17] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[17] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'i')
                    {
                        label19.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[18] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[18] + "\r\n";
                        count++;
                    }
                    if (input_string[i] == 'j')
                    {
                        label20.Text = Convert.ToString(count);
                        if (timer1.Enabled) textBox1.Text += commands[19] + "\r\n";
                        else textBox1.Text += "ФС\t" + commands[19] + "\r\n";
                        count++;
                    }
                }
                input_string = " ";

            }

            if (refresh_flag)
            {
                answ_p.SoundLocation = sound_answ_path;
                fals_p.SoundLocation = sound_fals_path;
                stop_p.SoundLocation = sound_stop_path;
                strt_p.SoundLocation = sound_strt_path;
                time_p.SoundLocation = sound_time_path;

                answ_p.Load();
                fals_p.Load();
                stop_p.Load();
                strt_p.Load();
                time_p.Load();

                refresh_flag = false;
            }
        }

        private void Scroll_text(object sender, EventArgs e)
        {
            textBox1.SelectionStart = textBox1.Text.Length;
            textBox1.ScrollToCaret();
        }

        private void dataGridView2_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click_1(object sender, EventArgs e)
        {

        }

        private void gameForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.S)       
            {
                if (button2.Text == "Старт")
                {
                    start_procedure();
                }
                else
                {
                    stop_procedure();
                }
            }
            if (e.KeyCode == Keys.N)
            {
                next_procedure();
            }
            if (e.KeyCode == Keys.Space)
            {
                //s_make();
            }
            if (e.KeyCode == Keys.R)
            {
                sbros_procedure();
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            
        }

        private void sbros_procedure()
        {
            timer1.Enabled = false;
            time_clock = 0;
            label1.Text = "00:00";
            count = 0;
            if (is_serial_port_open)
            {
                send_to_port = 11;
            }
            textBox1.Clear();

            label01.Text = " ";
            label02.Text = " ";
            label03.Text = " "; 
            label04.Text = " ";
            label05.Text = " "; 
            label06.Text = " ";
            label07.Text = " "; 
            label08.Text = " "; 
            label09.Text = " ";
            label10.Text = " ";
            label11.Text = " "; 
            label12.Text = " ";
            label13.Text = " ";
            label14.Text = " "; 
            label15.Text = " ";
            label16.Text = " ";
            label17.Text = " ";
            label18.Text = " ";
            label19.Text = " ";
            label20.Text = " ";
            button2.Text = "Старт";
        }

        private void next_procedure()
        {
            if (is_serial_port_open)
            {
                send_to_port = 12;
            }

            if (label01.Text != " ")
            {
                if (Convert.ToByte(label01.Text) == 0)
                {
                    label01.Text = " ";
                }
                else
                {
                    byte temp = Convert.ToByte(label01.Text);
                    label01.Text = Convert.ToString(temp--);
                }
            }

            if (label02.Text != " ")
            {
                if (Convert.ToByte(label02.Text) == 0)
                {
                    label02.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label02.Text);
                    temp--;
                    label02.Text = Convert.ToString(temp);
                }
            }

            if (label03.Text != " ")
            {
                if (Convert.ToByte(label03.Text) == 0)
                {
                    label03.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label03.Text);
                    temp--;
                    label03.Text = Convert.ToString(temp);
                }
            }

            if (label04.Text != " ")
            {
                if (Convert.ToByte(label04.Text) == 0)
                {
                    label04.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label04.Text);
                    temp--;
                    label04.Text = Convert.ToString(temp);
                }
            }

            if (label05.Text != " ")
            {
                if (Convert.ToByte(label05.Text) == 0)
                {
                    label05.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label05.Text);
                    temp--;
                    label05.Text = Convert.ToString(temp);
                }
            }

            if (label06.Text != " ")
            {
                if (Convert.ToByte(label06.Text) == 0)
                {
                    label06.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label06.Text);
                    temp--;
                    label06.Text = Convert.ToString(temp);
                }
            }

            if (label07.Text != " ")
            {
                if (Convert.ToByte(label07.Text) == 0)
                {
                    label07.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label07.Text);
                    temp--;
                    label07.Text = Convert.ToString(temp);
                }
            }

            if (label08.Text != " ")
            {
                if (Convert.ToByte(label08.Text) == 0)
                {
                    label08.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label08.Text);
                    temp--;
                    label08.Text = Convert.ToString(temp);
                }
            }

            if (label09.Text != " ")
            {
                if (Convert.ToByte(label09.Text) == 0)
                {
                    label09.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label09.Text);
                    temp--;
                    label09.Text = Convert.ToString(temp);
                }
            }

            if (label10.Text != " ")
            {
                if (Convert.ToByte(label10.Text) == 0)
                {
                    label10.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label10.Text);
                    temp--;
                    label10.Text = Convert.ToString(temp);
                }
            }

            if (label11.Text != " ")
            {
                if (Convert.ToByte(label11.Text) == 0)
                {
                    label11.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label11.Text);
                    temp--;
                    label11.Text = Convert.ToString(temp);
                }
            }
            if (label12.Text != " ")
            {
                if (Convert.ToByte(label12.Text) == 0)
                {
                    label12.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label12.Text);
                    temp--;
                    label12.Text = Convert.ToString(temp);
                }
            }
            if (label13.Text != " ")
            {
                if (Convert.ToByte(label13.Text) == 0)
                {
                    label13.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label13.Text);
                    temp--;
                    label13.Text = Convert.ToString(temp);
                }
            }
            if (label14.Text != " ")
            {
                if (Convert.ToByte(label14.Text) == 0)
                {
                    label14.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label14.Text);
                    temp--;
                    label14.Text = Convert.ToString(temp);
                }
            }
            if (label15.Text != " ")
            {
                if (Convert.ToByte(label15.Text) == 0)
                {
                    label15.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label15.Text);
                    temp--;
                    label15.Text = Convert.ToString(temp);
                }
            }
            if (label16.Text != " ")
            {
                if (Convert.ToByte(label16.Text) == 0)
                {
                    label16.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label16.Text);
                    temp--;
                    label16.Text = Convert.ToString(temp);
                }
            }
            if (label17.Text != " ")
            {
                if (Convert.ToByte(label17.Text) == 0)
                {
                    label17.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label17.Text);
                    temp--;
                    label17.Text = Convert.ToString(temp);
                }
            }
            if (label18.Text != " ")
            {
                if (Convert.ToByte(label18.Text) == 0)
                {
                    label18.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label18.Text);
                    temp--;
                    label18.Text = Convert.ToString(temp);
                }
            }
            if (label19.Text != " ")
            {
                if (Convert.ToByte(label19.Text) == 0)
                {
                    label19.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label19.Text);
                    temp--;
                    label19.Text = Convert.ToString(temp);
                }
            }
            if (label20.Text != " ")
            {
                if (Convert.ToByte(label20.Text) == 0)
                {
                    label20.Text = " ";
                }
                else
                {
                    byte temp;
                    temp = Convert.ToByte(label20.Text);
                    temp--;
                    label20.Text = Convert.ToString(temp);
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            sbros_procedure();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            next_procedure();
        }
    }
}
