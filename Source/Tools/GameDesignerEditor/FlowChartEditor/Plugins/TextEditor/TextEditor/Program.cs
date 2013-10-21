using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;
using DataElementLibrary;

namespace TextEditor
{
    [Serializable]
    public class TextEditor: DataElement
    {        
        /// <summary>
        /// 构造函数
        /// </summary>
        public TextEditor()
        {
           
        }

        /// <summary>
        /// 编辑数据
        /// </summary>
        /// <param name="table">输入数据</param>
        /// <return>是否编辑成功</return>
        public override bool EditData(Hashtable table)
        {
            bool editSuccess = false;
            string content = table["data"] as string;
            if(content == null)
            {
                content = "请输入编辑内容";
            }
            MainForm mainForm = new MainForm(content);
            DialogResult result = mainForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                text = mainForm.Content;
                data = mainForm.Content;                
                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        /// <return>是否保存成功</return>
        public override bool SaveData()
        {
            return true;
        }

        /// <summary>
        /// 删除数据
        /// </summary>
        /// <returns>是否删除成功</returns>
        public override bool DeleteData()
        {
            return true;
        }

        /// <summary>
        /// 重读数据
        /// </summary>
        /// <param name="table">重读参数</param>
        /// <return>是否重读成功</return>
        public override bool ReloadData(Hashtable table)
        {
            return false;
        }

        /// <summary>
        /// 获取输出的参数链表
        /// </summary>
        /// <returns>输出的参数链表</returns>
        public override Hashtable GetOutputArgs()
        {
            return null;
        }
    }
}