using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;

namespace LogicalEditor
{
    /// <summary>
    /// Summary description for CollectionPropertyDescriptor.
    /// </summary>
    public class LogicalObjectCollectionPropertyDescriptor : PropertyDescriptor
    {
        private LogicObj oldObj = null; // 初始化时初始化，装旧值，用来表示是否修改过，以便黑体显示。
        private LogicObj lobj = null;   // 随着修改，这个 obj 会改变，用来保存。
        private int index = -1;

        public LogicalObjectCollectionPropertyDescriptor(LogicObj lobj, int idx)
            :
            base("#" + idx.ToString(), null)
        {
            this.oldObj = lobj.Clone() as LogicObj;
            this.lobj = lobj;
            this.index = idx;
        }

        public override AttributeCollection Attributes
        {
            get
            {
                return new AttributeCollection(null);
            }
        }

        public override bool CanResetValue(object component)
        {
            return true;
        }

        public override Type ComponentType
        {
            get
            {
                return this.lobj.GetType();
            }
        }

        public override string DisplayName
        {
            get
            {
                return this.lobj.fields[index];
            }
        }

        public override string Description
        {
            get
            {
                return this.lobj.fields[index];
            }
        }

        public override object GetValue(object component)
        {
            return this.lobj.values[index];
        }

        public override bool IsReadOnly
        {
            get
            {
                if (this.lobj.fields[index].ToLower().Contains("scriptid"))
                    return true;
                else
                    return false;
            }
        }

        public override string Name
        {
            get { return "#" + index.ToString(); }
        }

        public override Type PropertyType
        {
            get { return this.lobj.values[index].GetType(); }
        }

        public override void ResetValue(object component)
        {
        }

        public override bool ShouldSerializeValue(object component) // 值是否改变，黑体显示用
        {
            bool inhandChanged = oldObj.values[index] != lobj.values[index];
            bool previewChanged = lobj.fields_changed[index];
            return inhandChanged || previewChanged;
            //return true;
        }

        public override void SetValue(object component, object value)
        {
            this.lobj.changed = true;
            if (this.lobj.fields[index].ToLower().Contains("filename") || this.lobj.fields[index].ToLower().Contains("scriptname"))
            {
                string newValue = value.ToString();
                string newInputValue = newValue;
                if (newValue.Contains(":")) // 是绝对路径
                    newInputValue = newValue.Substring(Form1.m_BasePath.Length);
                this.lobj.values[index] = newInputValue;

                if (this.lobj.fields[index].ToLower().Contains("scriptname"))
                {
                    for (int i = 0; i < this.lobj.fields.Length; i++)
                    {
                        if (this.lobj.fields[i].ToLower().Contains("scriptid"))
                        {
                            if (value.ToString().Length == 0)
                            {
                                this.lobj.values[i] = "00000000";
                            }
                            else
                            {
                                uint hashid = 0;
                                Program.Engine.GetFileNameHash(newInputValue, out hashid);
                                this.lobj.values[i] = hashid.ToString("X");
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                this.lobj.values[index] = value.ToString();
            }
        }

        public override object GetEditor(Type editorBaseType)
        {
            if (this.lobj.fields[index].ToLower().Contains("filename") || this.lobj.fields[index].ToLower().Contains("scriptname"))
            {
                return new System.Windows.Forms.Design.FileNameEditor();
            }
            else
            {
                return base.GetEditor(editorBaseType);
            }
        }
    }
}
