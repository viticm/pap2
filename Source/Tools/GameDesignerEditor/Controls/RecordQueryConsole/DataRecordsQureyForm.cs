using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Threading;


namespace RecordQueryConsole
{
    public partial class DataRecordsQureyForm : Form
    {
        private DataTable dtModel;
        private DataTable dtTab;
        private DataTable dtRecord;
        private SqlConnection Conn;
        private Thread t1 = null;
        public string strFileFullPath;

        public DataRecordsQureyForm(string databaseName)
        {
            InitializeComponent();
            this.Text = string.Format("修改记录查询（库：{0}）", databaseName);
            Init(databaseName);
        }

        private bool Init(string databaseName)
        {           
            string ConnString = string.Format("Server = 192.168.27.31; DataBase = {0}; Uid = s3design; Password = davidbowie;", databaseName);
            Conn = new SqlConnection(ConnString);
            dtModel = GetDataTable("select id, modelname_cn from sys_modl_def", Conn);
            dtTab = GetDataTable("select modtabid, modid, name from sys_modl_tab_def", Conn);            
            combxModelName.DataSource = dtModel;
            combxModelName.DisplayMember = "modelname_cn";
            combxModelName.ValueMember = "id";
            InitGridView();            
            return true;
        }
        private void InitGridView()
        {
            DataGridViewTextBoxColumn hostNameColumn = new DataGridViewTextBoxColumn();
            hostNameColumn.HeaderText = "用户名";
            hostNameColumn.Name = "hostname";

            DataGridViewTextBoxColumn editTimeColumn = new DataGridViewTextBoxColumn();
            editTimeColumn.HeaderText = "修改时间";
            editTimeColumn.Name = "edittime";

            DataGridViewTextBoxColumn modelNameColumn = new DataGridViewTextBoxColumn();
            modelNameColumn.HeaderText = "模块名";
            modelNameColumn.Name = "modelname";

            DataGridViewTextBoxColumn tabNameColumn = new DataGridViewTextBoxColumn();
            tabNameColumn.HeaderText = "属性页名";
            tabNameColumn.Name = "tabname";

            DataGridViewTextBoxColumn catNameColumn = new DataGridViewTextBoxColumn();
            catNameColumn.HeaderText = "所属分类";
            catNameColumn.Name = "catfields";

            DataGridViewTextBoxColumn recordNameColumn = new DataGridViewTextBoxColumn();
            recordNameColumn.HeaderText = "树结点名";
            recordNameColumn.Name = "nodename";

            DataGridViewTextBoxColumn fieldDisplayNameColumn = new DataGridViewTextBoxColumn();
            fieldDisplayNameColumn.HeaderText = "字段名";
            fieldDisplayNameColumn.Name = "fielddisplayname";

            DataGridViewTextBoxColumn oldValueColumn = new DataGridViewTextBoxColumn();
            oldValueColumn.HeaderText = "旧值";
            oldValueColumn.Name = "oldvalue";

            DataGridViewTextBoxColumn newValueColumn = new DataGridViewTextBoxColumn();
            newValueColumn.HeaderText = "新值";
            newValueColumn.Name = "newvalue";

            DataGridViewTextBoxColumn remarksColumn = new DataGridViewTextBoxColumn();
            remarksColumn.HeaderText = "备注";
            remarksColumn.Name = "remarks";

            gvDataRecord.Columns.Add(hostNameColumn);
            gvDataRecord.Columns.Add(editTimeColumn);
            gvDataRecord.Columns.Add(modelNameColumn);
            gvDataRecord.Columns.Add(tabNameColumn);
            gvDataRecord.Columns.Add(catNameColumn);
            gvDataRecord.Columns.Add(recordNameColumn);
            gvDataRecord.Columns.Add(fieldDisplayNameColumn);
            gvDataRecord.Columns.Add(oldValueColumn);
            gvDataRecord.Columns.Add(newValueColumn);
            gvDataRecord.Columns.Add(remarksColumn);
        }

        private DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
                

        private void btnQuery_Click(object sender, EventArgs e)
        {
            string SqlStr = "select * from sys_Datarecord ";
            SqlStr += ConstructWhere();
            dtRecord = GetDataTable(SqlStr, Conn);
            FillGridView();
        }
        private void FillGridView()
        {
            gvDataRecord.Rows.Clear();
            int nIndex = 0;
            foreach (DataRow r in dtRecord.Rows)
            {
                gvDataRecord.Rows.Add();
                gvDataRecord.Rows[nIndex].Cells["hostname"].Value = r["hostname"];
                gvDataRecord.Rows[nIndex].Cells["edittime"].Value = r["edittime"];
                gvDataRecord.Rows[nIndex].Cells["modelname"].Value = GetModelName(r["modelid"]);
                gvDataRecord.Rows[nIndex].Cells["tabname"].Value = GetTabName(r["tabid"]);
                gvDataRecord.Rows[nIndex].Cells["catfields"].Value = r["catfields"];
                gvDataRecord.Rows[nIndex].Cells["nodename"].Value = r["nodename"];
                gvDataRecord.Rows[nIndex].Cells["fielddisplayname"].Value = r["fielddisplayname"];
                gvDataRecord.Rows[nIndex].Cells["oldvalue"].Value = r["oldvalue"];
                gvDataRecord.Rows[nIndex].Cells["newvalue"].Value = r["newvalue"];
                gvDataRecord.Rows[nIndex].Cells["remarks"].Value = r["remarks"];
                nIndex++;

            }

        }

        /// <summary>
        /// 获取模块名称
        /// </summary>
        /// <param name="modeid">模块id</param>
        /// <returns>模块名称</returns>
        private string GetModelName(object modeid)
        {
            DataRow r = dtModel.Rows.Find(modeid);
            if (r != null)
            {
                return r["modelname_cn"].ToString();
            }
            else
            {
                return "找不到该模块";
            }
        }

        /// <summary>
        /// 获取属性页名称
        /// </summary>
        /// <param name="tabid">属性页id</param>
        /// <returns>属性页名称</returns>
        private string GetTabName(object tabid)
        {
            DataRow r = dtTab.Rows.Find(tabid);
            if (r != null)
            {
                return r["name"].ToString();
            }
            else
            {
                return "找不到该属性页";
            }
        }
       
        private string ConstructWhere()
        {
            string WhereString = string.Empty;
            DateTime startTime = dtpBegin.Value;
            DateTime endTime = dtpEnd.Value;
            DateTime newStartTime = new DateTime(startTime.Year, startTime.Month, startTime.Day, 0, 0, 0);
            DateTime newEndTime = new DateTime(endTime.Year, endTime.Month, endTime.Day, 23, 59, 59);

            WhereString = string.Format("Where edittime >= '{0}' And edittime <= '{1}'", newStartTime, newEndTime);
            if (cbxUserName.Checked)
            {
                WhereString += string.Format(" And hostname = '{0}'", tbxUserName.Text);
            }
            if (cbxModelName.Checked)
            {                
                WhereString += string.Format(" And modelid = '{0}'", combxModelName.SelectedValue);
            }
            if (cbxNodeName.Checked)
            {
                WhereString += string.Format(" And nodename like '%{0}%'", tbxNodeName.Text);
            }
            if (cbxFieldName.Checked)
            {
                WhereString += string.Format(" And fielddisplayname like '%{0}%'", tbxFieldName.Text);
            }

            return WhereString;
        }
       

        private void cbxModelName_CheckedChanged(object sender, EventArgs e)
        {
            combxModelName.Enabled = cbxModelName.Checked;           
        }

        private void cbxUserName_CheckedChanged(object sender, EventArgs e)
        {
            tbxUserName.Enabled = cbxUserName.Checked;
        }

        private void cbxNodeName_CheckedChanged(object sender, EventArgs e)
        {
            tbxNodeName.Enabled = cbxNodeName.Checked;
        }

        private void cbxFieldName_CheckedChanged(object sender, EventArgs e)
        {
            tbxFieldName.Enabled = cbxFieldName.Checked;
        }

        private void dataGridViewExportAsExcel(DataGridView dgv, string FileFullPath)
        {
            string strExamPath = FileFullPath.Substring(0, FileFullPath.LastIndexOf('\\'));
            if (!System.IO.Directory.Exists(strExamPath))
            {
                MessageBox.Show(FileFullPath, "目录错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Microsoft.Office.Interop.Excel.Application excelApplication = new Microsoft.Office.Interop.Excel.Application();
            excelApplication.EnableEvents = false;
            excelApplication.Application.DisplayAlerts = false;
            excelApplication.Workbooks.Add(true);
            Microsoft.Office.Interop.Excel.Worksheet myWorkSheet = (Microsoft.Office.Interop.Excel.Worksheet)excelApplication.ActiveSheet;
            excelApplication.Visible = false;
            int nRowIndex    = 0;
            int nColumnIndex = 0;
            object[,] strArr = new object[gvDataRecord.RowCount + 1, gvDataRecord.ColumnCount];
            foreach (DataGridViewColumn dgvc in dgv.Columns)
            {
                strArr[nRowIndex, nColumnIndex] = dgvc.HeaderText;
                ++nColumnIndex;
            }
            ++nRowIndex;
            nColumnIndex = 0;
           
            foreach (DataGridViewRow dgvr in dgv.Rows)
            {
                foreach (DataGridViewCell dgvcell in dgvr.Cells)
                {
                    strArr[nRowIndex, nColumnIndex] = dgvcell.Value.ToString();
                    ++nColumnIndex;                    
                }
                ++nRowIndex;
                nColumnIndex = 0;
            }
            
            string strExcelMaxColumnIndex = GetExcelMaxColumnIndex(dgv.ColumnCount, dgv.RowCount + 1);
            Microsoft.Office.Interop.Excel.Range myRange = (Microsoft.Office.Interop.Excel.Range)myWorkSheet.get_Range("A1",strExcelMaxColumnIndex);
            myRange.get_Resize(dgv.RowCount + 1, dgv.ColumnCount);
            try
            {
                myRange.Value2 = strArr;                
                myRange.Columns.AutoFit();                
                myRange.Cells.HorizontalAlignment = Microsoft.Office.Interop.Excel.XlHAlign.xlHAlignCenter;                

                myWorkSheet.SaveAs(FileFullPath, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                return;
            }
            MessageBox.Show("文件成功保存到了" + FileFullPath, "保存成功", MessageBoxButtons.OK, MessageBoxIcon.Information);

            excelApplication.Quit();            
            killexcel(excelApplication);            
            GC.Collect();
        }

        private void btExport_Click(object sender, EventArgs e)
        {
            if (gvDataRecord.Rows.Count == 0)
                return;
            SaveFileDialog dlgSaveFile = new SaveFileDialog();
            dlgSaveFile.Filter = "Excel Doucuments (*.xlsx) | *.xlsx | Xml Documents (*.xml) | *.xml";
            dlgSaveFile.DefaultExt = "xlsx";
            if (dlgSaveFile.ShowDialog() == DialogResult.OK)
            {
                strFileFullPath = dlgSaveFile.FileName; 
                t1 = new Thread(new ThreadStart(ThreadPorcExpot));
                t1.Start();
                //t1.Join();  
            }
        }
        private void ThreadPorcExpot()
        {
            string fileExten = System.IO.Path.GetExtension(strFileFullPath);
            if (fileExten == ".xml")
            {
                datagridviewExportAsXml(gvDataRecord, strFileFullPath);  
            }
            else if (fileExten == ".xlsx")
            {
                dataGridViewExportAsExcel(gvDataRecord, strFileFullPath);
            }
            else
            {
                MessageBox.Show("无法保存指定的扩展名，请选用可用的扩展名。");
            }

        }
        [DllImport("user32.dll")]
        private static extern int GetWindowThreadProcessId(IntPtr hwnd, out int id);
        public void killexcel(Microsoft.Office.Interop.Excel.Application xlapp)
        {
            try
            {
                IntPtr app = new IntPtr(xlapp.Hwnd);
                int processid;
                GetWindowThreadProcessId(app, out processid);
                System.Diagnostics.Process.GetProcessById(processid).Kill();
            }
            catch
            { }
        }
        private string GetExcelMaxColumnIndex(int nColumnCnt, int nRowCnt)
        {
            string strResult = string.Empty;
            char   cOne      = '\0';
            int    ncolcnt         = nColumnCnt;
            while (ncolcnt > 0)
            {
                cOne = (char)('A' + ncolcnt % 26 - 1);
                strResult = string.Format("{0}{1}", cOne, strResult);
                ncolcnt /= 26;
            }
            strResult = string.Format("{0}{1}", strResult, nRowCnt);
            return strResult;
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("真的退出？", "退出确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                this.Close();
            }
        }

        private void ckbxUserName_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox ckb = sender as CheckBox;
            gvDataRecord.Columns[ckb.Tag.ToString()].Visible = ckb.Checked;
        }

        private void datagridviewExportAsXml(DataGridView dgv, string FileFullPath)
        {
            string strExamPath = FileFullPath.Substring(0, FileFullPath.LastIndexOf('\\'));
            if (!System.IO.Directory.Exists(strExamPath))
            {
                MessageBox.Show(FileFullPath, "目录错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            DataSet ds4Xml = new DataSet();

            datagridviewToDataSet(dgv, ds4Xml);

            System.Xml.XmlTextWriter writer = new System.Xml.XmlTextWriter(FileFullPath, Encoding.UTF8);
            writer.Formatting = System.Xml.Formatting.Indented;
            writer.WriteStartDocument();
            string str = string.Format("type=\"text/xsl\" href=\"{0}.xslt\"", System.IO.Path.GetFileNameWithoutExtension(FileFullPath));
            writer.WriteProcessingInstruction("xml-stylesheet", str);

            ds4Xml.WriteXml(writer);
            writer.Close();
            MessageBox.Show("文件成功保存到了" + FileFullPath, "保存成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        private bool datagridviewToDataSet(DataGridView dgv, DataSet ds)
        {
            DataTable dt = new DataTable();
            dt.TableName = "OneRecord";
            foreach (DataGridViewTextBoxColumn gridcolumn in dgv.Columns)
            {
                DataColumn column = new DataColumn();
                column.ColumnName = gridcolumn.Name;
                dt.Columns.Add(column);
            }
            foreach (DataGridViewRow dgvr in dgv.Rows)
            {
                DataRow newRow = dt.NewRow();
                foreach (DataGridViewCell dgvcell in dgvr.Cells)
                {
                    newRow[dgvcell.ColumnIndex] = dgvcell.Value;
                }
                dt.Rows.Add(newRow);               
            }
            ds.Tables.Add(dt);
            ds.DataSetName = "DataRecords";

            return true;

        }

        
    }
}