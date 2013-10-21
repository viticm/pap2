#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#endregion License

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Fireball.CodeEditor.SyntaxFiles;
using System.Collections;
using Fireball.Syntax;

namespace FireEdit.Dialogs
{
    public partial class FireEditLanguageOptions : FireEdit.Dialogs.FireEditWindow
    {
         

        ArrayList _ModifiedLangs = null;

        Language _Current = null;

        public FireEditLanguageOptions()
        {
            InitializeComponent();

            _ModifiedLangs = new ArrayList();

            Dictionary<string,Language>.Enumerator langs = (Dictionary<string,Language>.Enumerator )FormMain.Languages.ListLanguages();

            while (langs.MoveNext())
            {
                cmbLanguages.Items.Add(langs.Current);
            }
            cmbLanguages.SelectedIndex = 0;

            
        }

        private void cmbLanguages_SelectedIndexChanged(object sender, EventArgs e)
        {
            Language lang = (Language)FormMain.Languages.GetFromIndex(cmbLanguages.SelectedIndex);

            _Current = lang;

            lstStyles.Items.Clear();

            TextStyle[] styles = lang.Styles;

            foreach (TextStyle current in styles)
            {
                lstStyles.Items.Add(current);
            }

            lstStyles.SelectedIndex = 0;
        }

        private void lstStyles_SelectedIndexChanged(object sender, EventArgs e)
        {
            propGrid.SelectedObject = lstStyles.SelectedItem;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            foreach (Language current in _ModifiedLangs)
            {
                current.SaveStyles();
            }

            this.Close();
        }

        private void propGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            _ModifiedLangs.Add(_Current);
        }
    }
}

