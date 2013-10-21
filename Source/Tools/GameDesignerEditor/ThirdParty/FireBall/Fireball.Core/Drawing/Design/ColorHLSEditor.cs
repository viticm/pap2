using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.ComponentModel;
using System.Windows.Forms.Design;
using System.Windows.Forms;

namespace Fireball.Drawing.Design
{
    public class ColorHLSEditor:UITypeEditor
    {
        

        private ColorHlsUI _colorUI;

        public ColorHLSEditor()
        {
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (provider != null)
            {
                IWindowsFormsEditorService winFormsService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
                if (winFormsService == null)
                {
                    return value;
                }
                if (this._colorUI == null)
                {
                    this._colorUI = new ColorHlsUI(this);
                }
                this._colorUI.Start(winFormsService, value);
                winFormsService.DropDownControl(this._colorUI);
                value = this._colorUI.Value;
                this._colorUI.End();
            }
            return value;
        }
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override bool IsDropDownResizable
        {
            get
            {
                return false;
            }
        }

    }
}
