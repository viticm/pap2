using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class RegionParamForm: Form
	{
        private RegionParam m_RegionParam;
        private Dictionary<string, RegionParam> m_ParamMap;
        public RegionParamForm(RegionParam param, Dictionary<string, RegionParam> paramMap)
		{
			InitializeComponent();
            m_RegionParam = param;
            m_ParamMap = paramMap;
		}

        private void OKBut_Click(object sender, EventArgs e)
        {
            if (m_ParamMap.ContainsKey(Text6.Text.Trim()))
            {
                string message = string.Format("Ãû³Æ{0}ÒÑ´æÔÚ", Text6.Text.Trim());
                MessageBox.Show(message);
                return;
            }
            m_RegionParam.PixelTolerance = Convert.ToInt32(Text1.Text);
            m_RegionParam.SampleTimes = Convert.ToInt32(Text2.Text);
            m_RegionParam.EliminateIsoDataCount = Convert.ToInt32(Text3.Text);
            m_RegionParam.TotalValue = Convert.ToInt32(Text4.Text);
            m_RegionParam.PixelCount = Convert.ToUInt32(Text5.Text);
            m_RegionParam.Name = Text6.Text;
            this.Close();
        }
	}
}