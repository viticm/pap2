using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using BaseExp;

namespace EventNode
{
    public partial class EventListForm : DevComponents.DotNetBar.Office2007Form
    {
        public GameEvent selectedEvent = null;
        private string m_strText = "";

        public EventListForm()
        {
            this.DialogResult = DialogResult.Cancel;
            InitializeComponent();
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            /*
            if ((this.selectedEvent!= null) 
                && (this.comboBox1.SelectedItem != null) 
                && this.selectedEvent.DBID == (this.comboBox1.SelectedItem as GameEvent).DBID)
            {
                this.Close();
                return;
            }*/
            
            this.selectedEvent = this.comboBox1.Items_SelectedItem as GameEvent;
            if (this.radioButtonCustom.Checked && expControl1.ActionExp != null)
            {
                if (this.expControl1.GetReady)
                    this.m_strText = expControl1.ActionExp.ToString();
                else
                {
                    MessageBox.Show("请输入完整", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            else
                this.m_strText = selectedEvent.ToString();
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        public override string ToString()
        {
            return m_strText;
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void EventListForm_Load(object sender, EventArgs e)
        {
            GameEvent[] events = ExpManager.GetGameEventList();

            //用带数据的事件替换事件列表中的事件
            for (int i = 0; i < events.Length; i++)
            {
                if (selectedEvent != null && events[i].DBID == selectedEvent.DBID)
                {
                    events[i] = selectedEvent.Clone() as GameEvent;
                    break;
                }
            }

            this.comboBox1.Items_AddRange(events);

            //自动选中
            foreach(GameEvent i_event in comboBox1.Items_All)
            {
                if(selectedEvent != null && i_event != null && selectedEvent.DBID == i_event.DBID)
                {
                    comboBox1.Items_SelectedItem = i_event;
                    break;
                }
            }
            if (comboBox1.Items_SelectedItem == null && comboBox1.Items_Count > 0)
                comboBox1.Items_SelectedItem = comboBox1.Items_All[0];

            //自动选中全部/指定
            GameEvent selEvent = this.comboBox1.Items_SelectedItem as GameEvent;
            if (selEvent != null)
            {
                EventAPI api = selEvent.GetEventAPI();
                bool isNull = false;
                for (int i = 0; i < api.ArgValues.Count; i++)
                {
                    if (api.ArgValues[i] != null) isNull = true;
                }
                if (isNull)
                {
                    this.radioButtonCustom.Checked = true;
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!radioButtonCustom.Checked) return;
            if (comboBox1.Items_SelectedItem == null) return;
            GameEvent selEvent = this.comboBox1.Items_SelectedItem as GameEvent;
            expControl1.SetComboText(selEvent);
        }

        private void radioButtonCustom_CheckedChanged(object sender, EventArgs e)
        {
            expControl1.Enabled = radioButtonCustom.Checked;
            if(radioButtonCustom.Checked)
            {
                if (comboBox1.Items_SelectedItem == null) return;
                GameEvent selEvent = this.comboBox1.Items_SelectedItem as GameEvent;
                expControl1.SetComboText(selEvent);
            }
            else
            {
                //去掉自定义信息
                EventAPI api = null ;
                if (this.comboBox1.Items_SelectedItem != null)
                {
                    api = (this.comboBox1.Items_SelectedItem as GameEvent).GetEventAPI();
                    for (int i = 0; i < api.ArgList.Length; i++)
                        api.ArgValues[i] = null;
                }
            }
        }
    }
}