using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace test
{
    public partial class Form1 : Form
    {   
        public Form1()
        {
            InitializeComponent();
            button6.BackColor = System.Drawing.Color.SkyBlue;
        }

        private void button5_Click(object sender, EventArgs e) //카메라
        {

            button5.BackColor = System.Drawing.Color.Azure;
            button6.BackColor = System.Drawing.Color.SkyBlue;
            panel5.Visible = false;
           
            panel6.Visible = true;
            panel7.Visible = true;
            panel8.Visible = true;
            panel9.Visible = true;
        }

        private void button6_Click(object sender, EventArgs e) // 위치
        {

            button5.BackColor = System.Drawing.Color.SkyBlue;
            button6.BackColor = System.Drawing.Color.Azure;
            panel5.Visible = true;
         
            panel6.Visible = false;
            panel7.Visible = false;
            panel8.Visible = false;
            panel9.Visible = false;
        }


    }
}
