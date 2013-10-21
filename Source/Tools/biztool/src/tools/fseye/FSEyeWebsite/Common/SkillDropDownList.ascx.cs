using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using Resources;
using FSEye.FS2Lib;

public partial class Common_SkillDropDownList : System.Web.UI.UserControl
{
	int _maxSkillPerPage = 100;
	
	/// <summary>
	/// 每页显示技能的最大个数
	/// </summary>
	public int MaxSkillPerPage
	{
		get { return _maxSkillPerPage; }
		set { _maxSkillPerPage = value; }
	}

	/// <summary>
	/// 选中的技能
	/// </summary>
	public FS2SkillData SelectedSkill
	{
		get
		{
			try
			{
				return FS2GameDataManager.TheInstance.GetSkillData(int.Parse(DropDownListSkill.SelectedValue));
			}
			catch (Exception)
			{
				return null;
			}
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!IsPostBack)
		{
			CreatePageList();
			CreateSkillList();
		}
	}

	void CreatePageList()
	{
		DropDownListPage.Items.Clear();

		FS2SkillData[] skillDatas = FS2GameDataManager.TheInstance.GetAllSkillData();
		if (skillDatas != null && skillDatas.Length > 0)
		{
			if (skillDatas.Length <= _maxSkillPerPage)
				DropDownListPage.Visible = false;

			int pageCount = skillDatas.Length / _maxSkillPerPage + 1;
			for (int i = 0; i < pageCount; i++)
			{
				DropDownListPage.Items.Add(new ListItem(string.Format("第{0}页", i + 1), i.ToString()));
			}
		}
		else
		{
			DropDownListPage.Visible = false;
		}
	}

	void CreateSkillList()
	{
		DropDownListSkill.Items.Clear();

		DropDownListSkill.Items.Add(new ListItem(StringDef.Unspecified, "0"));
		FS2SkillData[] skillDatas = FS2GameDataManager.TheInstance.GetAllSkillData();
		int page = 0;
		try
		{
			page = int.Parse(DropDownListPage.SelectedValue);
		}
		catch (Exception)
		{
		}
		int startIndex = _maxSkillPerPage * page;
		int endIndex = _maxSkillPerPage * (page + 1);
		if (endIndex > skillDatas.Length)
			endIndex = skillDatas.Length;
		for (int i = startIndex; i < endIndex; i++)
		{
			FS2SkillData skillData = skillDatas[i];
			if (skillData != null)
				DropDownListSkill.Items.Add(new ListItem(skillData.ToString(), skillData.TemplateId.ToString()));
		}
	}

	protected void DropDownListPage_SelectedIndexChanged(object sender, EventArgs e)
	{
		CreateSkillList();
	}
}
