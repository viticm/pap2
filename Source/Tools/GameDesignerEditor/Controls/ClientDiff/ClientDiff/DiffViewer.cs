using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace ClientDiff
{
    class DiffViewer
    {
        static public bool DiffView(string filename, string postfix, string keyfields, string namefields, string output, string output_basic, List<string> diffCols1, List<string> diffCols2)
        {
            DiffViewForm frmView = new DiffViewForm(filename, postfix, keyfields, namefields, output, output_basic, diffCols1, diffCols2);
            frmView.ShowDialog();

            return true;
        }

    }
}
