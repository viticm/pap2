using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

using CustomControls;
using CustomControls.OS;
using CustomControls.Controls;

namespace IconBrowser
{
    public class Implement
    {
        Rectangle m_rect;
        int m_id;
        string m_filename;
        public Rectangle Rect
        {
            set {m_rect = value;}
            get { return m_rect; }
        }
        public int ID
        {
            get { return m_id; }
            set { m_id = value; }
        }
        public string FileName
        {
            set { m_filename = value; }
            get { return m_filename; }
        }

        public string Show()
        {
            FormOpenFileDialog controlex            = new FormOpenFileDialog();
            controlex.StartLocation                 = AddonWindowLocation.Right;
            controlex.DefaultViewMode               = FolderViewMode.Default;
            controlex.OpenDialog.InitialDirectory = Path.GetFullPath(AppDomain.CurrentDomain.BaseDirectory);//Application.ExecutablePath);
            controlex.OpenDialog.AddExtension       = true;
            controlex.OpenDialog.Filter             = "½£3Í¼±êÎÄ¼þ(*.UITex)|*.UITex";//"Image Files(*.bmp;*.jpg;*.gif;*.png)|*.bmp;*.jpg;*.gif;*.png";

            //AppDomain.CurrentDomain.BaseDirectory;
            //controlex.SetCurrenIconRect(Rect);
            controlex.InitRect = Rect;
            DialogResult r = controlex.ShowDialog(null, FileName);
            if (r == DialogResult.OK)
            {
                Rect = controlex.Rect;
                ID = controlex.ID;
                return controlex.OpenDialog.FileName;
            }
            controlex.UnInit();
            controlex.Dispose();
            return string.Empty;
        }
    }
}
