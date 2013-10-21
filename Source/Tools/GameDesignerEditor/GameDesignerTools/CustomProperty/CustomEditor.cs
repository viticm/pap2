using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms.Design;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Design;
using System.Drawing;
using System.Windows.Forms.VisualStyles;
using System.Drawing.Drawing2D;

namespace GameDesingerTools
{
    class FileNameEditorEx : FileNameEditor
    {
        private object NewEditValue(object filename, string filenamefileter)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.InitialDirectory = filename.ToString();
            if (Path.GetFileName(filename.ToString()).Length > 0)
                openFileDialog1.FileName = filename.ToString();
            openFileDialog1.Filter = "模型文件(*.mdl)|*.mdl|脚本文件(*.lua)|*.lua|所有文件(*.*)|*.*";
            openFileDialog1.FilterIndex = 3;
            openFileDialog1.RestoreDirectory = true;
            if (filenamefileter != null && filenamefileter.Trim().Length > 0 && (filenamefileter.IndexOf('*') >= 0 || filenamefileter.IndexOf('?') >= 0))
            {
                openFileDialog1.Filter = string.Format("{0}|{0}|{1}", filenamefileter, openFileDialog1.Filter);
                openFileDialog1.FilterIndex = 0;
            }

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                return openFileDialog1.FileName;
            }
            return filename;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context != null &&
                provider != null && 
                context.PropertyDescriptor != null && 
                context.PropertyDescriptor is CustomPropertyDescriptor)
            {
                return DoEditValue((CustomPropertyDescriptor)(context.PropertyDescriptor), value);
            }
            else
                return base.EditValue(context, provider, value);
        }

        public object DoEditValue(CustomPropertyDescriptor descriptor, object value)
        {
            string strPrePath;// = ((CustomPropertyDescriptor)context.PropertyDescriptor).Property.Prefix;
            string filenamefilter = Path.GetFileName(descriptor.Property.Prefix);
            //oldvalue 要处理为绝对路径
            string stroldvalue = value.ToString().Trim();
            object oldvalue;
            string strRootDir = Helper.AddSlash(Program.RootDir);
            string strCutDir;
            if (stroldvalue.Length > 0) //已经有值,则直接与rootdir组合
            {
                oldvalue = Helper.CombinePaths(strRootDir, stroldvalue);
                strPrePath = strRootDir;
                strCutDir = Helper.AddSlash(descriptor.Property.CutPrefix);
            }
            else
            {
                strPrePath = descriptor.Property.Prefix;
                strCutDir = Helper.AddSlash(descriptor.Property.CutPrefix);
                oldvalue = Helper.CombinePaths(strPrePath, stroldvalue);
            }

            object newvalue = NewEditValue(oldvalue, filenamefilter); //base.EditValue(context, provider, oldvalue);
            if (!newvalue.Equals(oldvalue) && newvalue != oldvalue)
            {
                //newvalue要处理成相对路径
                //string[] newvaluearray = Helper.split(newvalue.ToString().ToLower(), strPrePath.ToLower());
                //string[] newvaluearray = Helper.split(newvalue.ToString().ToLower(), strRootDir.ToLower());
                string[] newvaluearray = Helper.split(newvalue.ToString().ToLower(), strCutDir.ToLower());
                if (newvaluearray != null && newvaluearray.Length > 1)
                {
                    string strvalue = newvalue.ToString();
                    newvalue = strvalue.Substring(strvalue.Length - newvaluearray[1].Length);
                    newvalue = descriptor.Property.CutPreSlash ? newvalue : ("\\" + newvalue);
                }
                else
                {
                    //return value; //不正确的路径文件，还原为原始文件
                    throw new Exception("不正确的文件，可能是您没有选择正确路径下的文件。");
                }
            }
            else
                return value;//还原为原始的相对的路径
            return newvalue;
        }
    }
    public class CustomEditor : System.Drawing.Design.UITypeEditor
    {
        private IWindowsFormsEditorService edSvc = null;

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            bool bCanEdit = Program.MainForm.m_CurBaseForm.CanSaveDB;

            if (!Program.m_bLockDBForCompetition &&
                //(!Program.m_bUseNewAcl || bCanEdit) &&
                context != null &&
                provider != null &&
                context.Instance != null &&
                context.PropertyDescriptor != null &&
                context.PropertyDescriptor is CustomPropertyDescriptor)
            {
                edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));

                if (edSvc != null)
                {
                    //edSvc.DropDownControl(new TextBox());
                    //SetEditorProps((FlashTrackBar)context.Instance, trackBar);
                    //bool asInt = true;
                    //edSvc.ShowDialog(null);
                    CustomProperty prop = ((CustomPropertyDescriptor)context.PropertyDescriptor).Property;
                    if (context.Instance is CustomClass)
                    {
                        return DoEditValue((CustomClass)context.Instance, (CustomPropertyDescriptor)context.PropertyDescriptor, value);
                    }
                }
            }

            return value;
        }

        public object DoEditValue(CustomClass instance, CustomPropertyDescriptor descriptor, object value)
        {
            object newvalue = instance.OnCustomEditValue(descriptor.Property, value);
            if (value is CustomClass)
            {
                object newrealvalue = newvalue is CustomClass ? (newvalue as CustomClass).Value : newvalue;
                if (!(value as CustomClass).Value.Equals(newrealvalue))
                    descriptor.Property.SetValue(newrealvalue);
            }
            return newvalue == null ? value : newvalue;
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null && context.Instance != null)
            {
                return UITypeEditorEditStyle.Modal;
            }
            return base.GetEditStyle(context);
        }
    }
    public class TextEditor : System.Drawing.Design.UITypeEditor
    {
        //private Brush br = new SolidBrush(Color.FromArgb(50, Color.Gray));
        public override void PaintValue(PaintValueEventArgs e)
        {
            base.PaintValue(e);

            //e.Graphics.FillRectangle(br, e.Bounds);
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            return value;
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null && context.Instance != null)
            {
                return UITypeEditorEditStyle.Modal;
            }
            return base.GetEditStyle(context);
        }
    }
    public class BoolEditor : System.Drawing.Design.UITypeEditor
    {
        //private Brush br = new SolidBrush(Color.FromArgb(50, Color.Red));
        private Point m_pt = new Point();
        public BoolEditor()
        {            
        }
        public override void PaintValue(PaintValueEventArgs e)
        {
            //base.PaintValue(e);
            //e.Graphics.FillRectangle(br, e.Bounds);
            
            if(m_pt.X == 0 && m_pt.Y == 0)
            {
                m_pt.X = e.Bounds.Left + (e.Bounds.Width - CheckBoxRenderer.GetGlyphSize(e.Graphics, CheckBoxState.CheckedNormal).Width) / 2;
                m_pt.Y = e.Bounds.Top + (e.Bounds.Height - CheckBoxRenderer.GetGlyphSize(e.Graphics, CheckBoxState.UncheckedNormal).Height) / 2;
            }
            
            CheckBoxRenderer.DrawCheckBox(e.Graphics,
                m_pt, e.Bounds, null,
                null, TextFormatFlags.HorizontalCenter,
                false, 
                e.Value.ToString() == "是"  ? CheckBoxState.CheckedNormal : CheckBoxState.UncheckedNormal);            
        }
        public override bool GetPaintValueSupported(System.ComponentModel.ITypeDescriptorContext context)
        {
            return true;
        }
        /*public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            return value;
        }*/

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null && context.Instance != null)
            {
                return UITypeEditorEditStyle.None; //交由系统缺省处理
            }
            return base.GetEditStyle(context);
        }
    }
}
