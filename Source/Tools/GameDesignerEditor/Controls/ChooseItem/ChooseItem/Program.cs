using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace ChooseItem
{
    public class Program
    {
        private Form1 form1; // 选择窗口
        private List<string> tableNames = new List<string>(); // 表名链表
        private List<string> detailList = new List<string>(); // Detail键值链表
        private List<string> particularList = new List<string>(); // Particular键值对链表

        /// <summary>
        /// genre属性
        /// </summary>
        public string Genre
        {
            get
            {
                return form1.Genre;
            }
        }

        /// <summary>
        /// detail属性
        /// </summary>
        public string Detail
        {
            get
            {
                return form1.Detail;
            }
        }

        /// <summary>
        /// particular属性
        /// </summary>
        public string Particular
        {
            get
            {
                return form1.Particular;
            }
        }

        /// <summary>
        /// level属性
        /// </summary>
        public string Level
        {
            get
            {
                return form1.Level;
            }
        }

        /// <summary>
        /// itemName属性
        /// </summary>
        public string ItemName
        {
            get
            {
                return form1.ItemName;
            }
        }
        
        /// <summary>
        /// 增加表
        /// </summary>
        /// <param name="tableName"></param>
        public void AddTable(string tableName)
        {
            tableNames.Add(tableName);
        }

        /// <summary>
        /// 给Detail链表添加新值
        /// </summary>
        /// <param name="value"></param>
        public void AddDetailList(string value)
        {
            detailList.Add(value);
        }

        /// <summary>
        /// 给Particual链表添加新值
        /// </summary>
        /// <param name="value"></param>
        public void AddParticularList(string value)
        {
            particularList.Add(value);
        }

        /// <summary>
        /// 显示批量添加删除窗口
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="id">id号</param>
        /// <param name="genre">genre字段</param>
        /// <param name="detail">detail字段</param>
        /// <param name="particular">particular字段</param>
        /// <param name="level">level字段</param>
        /// <returns>用户是否确认选择</returns>
        public int Show(SqlConnection conn, string id, object genre, object detail, object particular, object level)
        {
            if((genre is DBNull) || (detail is DBNull) || (particular is DBNull) || level is DBNull)
            {
                genre = "0";
                detail = "0";
                particular = "0";
                level = "0";
            }

            if(genre.ToString() == "-1" || detail.ToString() == "-1" || particular.ToString() == "-1" 
                || level.ToString() == "-1") // 是否是默认行数据
            {
                return 0;
            }

            Hashtable particularTable = new Hashtable();
            Hashtable detailTable = new Hashtable();

            if(particularList.Count != 0) // particular链表不为空
            {
                int index = 1;
                foreach(string s in particularList)
                {
                    particularTable[index.ToString()] = s;
                    index++;
                }
            }

            if(detailList.Count != 0) // detail链表不为空
            {
                int index = 1;
                foreach(string s in detailList)
                {
                    detailTable[index.ToString()] = s;
                    index++;
                }
            }

            form1 = new Form1(conn, tableNames, detailTable, particularTable);
            form1.ID = id;
            form1.Genre = genre.ToString();
            form1.Detail = detail.ToString();
            form1.Particular = particular.ToString();
            form1.Level = level.ToString();
            form1.InitSelection(); // 初始化选择
            DialogResult result = form1.ShowDialog();
            if(result == DialogResult.OK)
            {
                return 1; // 用户确认选择
            }
            else
            {
                return 0; // 用户未确认选择
            }
        }
    }
}