using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;


namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        SerialPort port;
        string comPort;
        
        public Form1()
        {

            try
            {
                // You can use this code to read the COMPORT from a text file, if you want.
                //StreamReader textFile = new StreamReader("config.txt");
                //comPort = textFile.ReadToEnd();
                //textFile.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Can't find file :: " + ex.Message, "Error!"); 
            }

        try
        {
            InitializeComponent();
            comPort = "COM7"; //the comport the Arduino is connected to... you will have to change this most likely
            port = new SerialPort(comPort, 9600, Parity.None, 8, StopBits.One);
            port.DtrEnable = true;
            port.Open();

  
        }
        catch (Exception /*ex*/)
        {
            textBox1.Text = "Can't Open COM7"; // This will be displayed if the program can't open the comport you have assigned (such as if the Arduino is not connected or you assigned the wrong port)
            //MessageBox.Show("Error opening/writing to serial port :: " + ex.Message, "Error!"); 
        }

        if (port.IsOpen) // These just toggle the visible state of the buttons depending on whether you're connected to the Arduino or not.
        {
            button1.Enabled = true;
            button2.Enabled = true;
            textBox1.ReadOnly = false;
        }
        else
        {
            button1.Enabled = false;
            button2.Enabled = false;
            textBox1.ReadOnly = true;
        }

        }

        private void button1_Click(object sender, EventArgs e)
        {
                if (!port.IsOpen) return; //if port is not open, don't do anything

                port.Write("7001"); // This can be almost any number you want. You can even try playing with characters too.
                textBox1.Text = "ON"; // This will update the text box with the word "ON".
                UpdateData(false);

        }

        private void button2_Click(object sender, EventArgs e)
        {
                if(!port.IsOpen) return; //if port is not open, don't do anything

                port.Write("7002");
                textBox1.Text = "Off";
            
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            
        }

        // Try and Reset the SerialPort
        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                port.Open();
                if (port.IsOpen)
                {
                    button1.Enabled = true;
                    button2.Enabled = true;
                    textBox1.ReadOnly = false;
                }

            }
            catch (Exception /*ex*/)
            {
                textBox1.Text = "Can't Open COM7";
                //MessageBox.Show("Error opening/writing to serial port :: " + ex.Message, "Error!");
            }     
        }
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (port.IsOpen) port.Close();
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            port.Write(trackBar1.Value.ToString());
            textBox2.Text = trackBar1.Value.ToString();
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }


    }
}
