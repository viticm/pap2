using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace DataRecordsQurey
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            SelectDataBaseForm dbForm = new SelectDataBaseForm();
            if (dbForm.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            string ChoosedDBName = dbForm.ChoosedDBName;
            Application.Run(new DataRecordsQureyForm(ChoosedDBName));
        }
    }
}