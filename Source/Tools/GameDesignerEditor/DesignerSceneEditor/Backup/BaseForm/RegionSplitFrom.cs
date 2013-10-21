using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{

	public partial class RegionSplitFrom: Form
	{
        private Dictionary<string, RegionParam> m_RegionParamDic;
        private string m_EditName = string.Empty;
        private AtlKG3DEngineProxyLib.SceneEditor m_SceneEditor;

		public RegionSplitFrom(AtlKG3DEngineProxyLib.SceneEditor editor)
		{
			InitializeComponent();
            m_RegionParamDic = new Dictionary<string, RegionParam>();
            m_SceneEditor = editor;
		}

        private bool ExistFile(string file)
        {
            bool bRet = false;
            foreach (RegionParam param in m_RegionParamDic.Values)
            {
                if (param.Path == file)
                {
                    bRet = true;
                    break;
                }
            }
            return bRet;
        }

        private void AddBut_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            
            dlg.Filter = "Texture Files(*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS)|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS";
           
            if (dlg.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            if (ExistFile(dlg.FileName))
            {
                string message = string.Format("{0} 该文件已添加", dlg.FileName);
                MessageBox.Show(message);
                return;
            }
            RegionParam param = new RegionParam();
            param.Path = dlg.FileName;
            RegionParamForm frm = new RegionParamForm(param, m_RegionParamDic);
            frm.ShowDialog();
            m_RegionParamDic.Add(param.Name, param);
            ListViewItem item = new ListViewItem(param.Name);
            TargetListView.Items.Add(item);
            uint handle = 0;
            m_SceneEditor.SetFocus();
            m_SceneEditor.AddRegionInfoManager(param.Path, param.PixelTolerance, param.SampleTimes,
                param.EliminateIsoDataCount, param.TotalValue, param.PixelCount, ref handle);
            param.Handle = handle;
            item.Selected = true;
            
        }

        private void DelBut_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            foreach (ListViewItem item in items)
            {
                uint handle = m_RegionParamDic[item.Text].Handle;
                m_SceneEditor.SetFocus();
                m_SceneEditor.ShowRegionInfoMap(0, 0, 0);
                m_SceneEditor.DelRegionInfoManager(handle);
                m_RegionParamDic.Remove(item.Text);
                TargetListView.Items.Remove(item);
                this.Refresh();
            }
        }


        private bool ShowRegionInfoMap(uint handle, bool bShow)
        {
            int nShow = bShow ? 1 : 0;
            m_SceneEditor.SetFocus();
            m_SceneEditor.ShowRegionInfoMap(panelEx1.Handle.ToInt32(), handle, nShow);
            this.Refresh();
            return true;
        }

        private bool ShowRegionInfo(string name)
        {
            uint uHeight = 0;
            uint uWidth = 0;
            uint uPixelPerRegion = 0;
            uint uRegionCount = 0;
            uint uMemorySize = 0;

            uint handle = m_RegionParamDic[name].Handle;
            m_SceneEditor.GetRegionMapInfo(handle, ref uHeight, ref uWidth, ref uPixelPerRegion,
                ref uRegionCount, ref uMemorySize);

            HeightBox.Text = uHeight.ToString();
            WidthBox.Text = uWidth.ToString();
            PixelCountBox.Text = uPixelPerRegion.ToString();
            RegionCountBox.Text = uRegionCount.ToString();
            MemoryBox.Text = uMemorySize.ToString();

            PathBox.Text = m_RegionParamDic[name].Path;

            RegionListView.Clear();
            int value = 0;
            for (uint i = 0; i < uRegionCount; ++i)
            {
                m_SceneEditor.GetRegionDiffValue(handle, i, ref value);
                if (value == -1)
                {
                    break;
                }
                string diff = ColorToString(value);
                ListViewItem item = new ListViewItem(diff);
                RegionListView.Items.Add(item);
            }

            m_SceneEditor.GetRegionDiffValue(handle, uRegionCount, ref value);


            return true;
        }

        private string ColorToString(int color)
        {
            long a = (color & 0xff000000) >> 24;
            long r = (color & 0x00ff0000) >> 16;
            long g = (color & 0x0000ff00) >> 8;
            long b = color & 0x000000ff;
            string ret = string.Format("{0},{1},{2},{3}", a, r, g, b);
            return ret;
        }

        private void TargetListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            if (items.Count == 0)
            {
                ShowRegionInfoMap(0, false);
            }
            else
            {
                ListViewItem item = items[0];
                string name = item.Text;
                RegionParam param = m_RegionParamDic[name];
                ShowRegionInfoMap(param.Handle, true);
                ShowRegionInfo(name);

                RenderToTerrainChk_CheckedChanged(null, null);
            }
        }

        private void RenderToTerrainChk_CheckedChanged(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            if (items.Count == 0)
            {
                return;
            }

            uint handle = m_RegionParamDic[items[0].Text].Handle;
            int nEnable = RenderToTerrainChk.Checked ? 1 : 0;
            m_SceneEditor.SetFocus();
            m_SceneEditor.EnableRegionMask(handle, nEnable);
        }

        private void RegionSplitFrom_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void SaveBut_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            if (items.Count == 0)
            {
                return;
            }

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Texture Files(*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS)|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS";
            if (dlg.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            RegionParam param = m_RegionParamDic[items[0].Text];
            m_SceneEditor.SetFocus();
            m_SceneEditor.SaveRegionMap(dlg.FileName, param.Handle);
            param.Path = dlg.FileName;
            m_SceneEditor.DelRegionInfoManager(param.Handle);

            uint handle = 0;
            m_SceneEditor.AddRegionInfoManager(param.Path, param.PixelTolerance, param.SampleTimes, param.EliminateIsoDataCount,
                param.TotalValue, param.PixelCount, ref handle);

            param.Handle = handle;
            ShowRegionInfoMap(handle, true);
            ShowRegionInfo(param.Name);
            RenderToTerrainChk_CheckedChanged(null, null);

        }

        private void NameBut_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            if (items.Count == 0)
            {
                return;
            }

            m_EditName = items[0].Text;
            items[0].BeginEdit();
        }

        private void TargetListView_AfterLabelEdit(object sender, LabelEditEventArgs e)
        {

            if (e.Label == null)
            {
                return;
            }
            string name = e.Label.Trim();
            if (m_EditName == name)
            {
                return;
            }
            if (name == string.Empty)
            {
                string error = "名字不能为空";
                MessageBox.Show(error);
                TargetListView.Items[e.Item].Text = m_EditName;
                return;
            }

            if (m_RegionParamDic.ContainsKey(name))
            {
                string error = string.Format("名称{0}已存在", name);
                MessageBox.Show(error);
                TargetListView.Items[e.Item].Text = m_EditName;
                return;
            }

            RegionParam param = m_RegionParamDic[m_EditName];
            param.Name = name;
            m_RegionParamDic.Remove(m_EditName);
            m_RegionParamDic[name] = param;
        }

        private void TargetListView_BeforeLabelEdit(object sender, LabelEditEventArgs e)
        {
            m_EditName = TargetListView.Items[e.Item].Text;
        }

        private void RereadBut_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = TargetListView.SelectedItems;
            if (items.Count == 0)
            {
                return;
            }
            RegionParam param = m_RegionParamDic[items[0].Text];
            m_SceneEditor.SetFocus();
            m_SceneEditor.DelRegionInfoManager(param.Handle);

            uint handle = 0;
            m_SceneEditor.AddRegionInfoManager(param.Path, param.PixelTolerance, param.SampleTimes, param.EliminateIsoDataCount,
                param.TotalValue, param.PixelCount, ref handle);

            param.Handle = handle;
            ShowRegionInfoMap(handle, true);
            ShowRegionInfo(param.Name);
            RenderToTerrainChk_CheckedChanged(null, null);
        }




	}

    public class RegionParam
    {
        public int PixelTolerance = 0;
        public int SampleTimes = 0;
        public int EliminateIsoDataCount = 0;
        public int TotalValue = 0;
        public uint PixelCount = 0;
        public uint Handle;
        public string Name = string.Empty;
        public string Path = string.Empty;
    }
}