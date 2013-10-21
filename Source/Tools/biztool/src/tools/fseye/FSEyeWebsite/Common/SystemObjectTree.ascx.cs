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
using FSEye;
using FSEye.Security;

public partial class Common_SystemObjectTree : System.Web.UI.UserControl
{
	public event EventHandler SelectedObjectChanged;

	public FSEyeObject SelectedObject
	{
		get
		{
			string selectedValue = TreeViewSystemObject.SelectedValue;
			if (selectedValue.Length > 0)
			{
				int objectId = int.Parse(selectedValue);
				return AdminServer.TheInstance.SecurityManager.Get(objectId);
			}

			return null;
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!IsPostBack)
        {
            TreeNode rootNode = new TreeNode(AdminServer.TheInstance.SecurityManager.Root.Name,
                                             AdminServer.TheInstance.SecurityManager.Root.Id.ToString());
            TreeViewSystemObject.Nodes.Add(rootNode);           
			BuildObjectTree(rootNode,AdminServer.TheInstance.SecurityManager.Root);
        }
	}

	void BuildObjectTree(TreeNode node,FSEyeObject root)
	{
        if (root.Children != null)
        {
            foreach (FSEyeObject child in root.Children)
            {
                TreeNode childNode = new TreeNode(child.Name, child.Id.ToString());
                childNode.PopulateOnDemand = true;
                node.ChildNodes.Add(childNode);
                //BuildObjectTree(childNode, child);
            }
        }
        //TreeViewSystemObject.Nodes.Clear();
        //TreeNode node = new TreeNode(root.Name, root.Id.ToString());
        //TreeViewSystemObject.Nodes.Add(node);
	}

	TreeNode CreateChildNode(FSEyeObject obj)
	{
		if (obj == null)
			throw new ArgumentNullException("obj");

		TreeNode node = new TreeNode(obj.Name, obj.Id.ToString());
		foreach (FSEyeObject child in obj.Children)
		{
			node.ChildNodes.Add(CreateChildNode(child));
		}

		return node;
	}

	protected void TreeViewSystemObject_SelectedNodeChanged(object sender, EventArgs e)
	{
        //TreeNode selectedNode = TreeViewSystemObject.SelectedNode;
        //if (selectedNode != null)
        //{
        //    FSEyeObject selectedObj = SelectedObject;
        //    if (selectedObj != null)
        //    {
        //        if (selectedNode.ChildNodes.Count != selectedObj.Children.Length)
        //        {
        //            selectedNode.ChildNodes.Clear();

        //            foreach (FSEyeObject child in selectedObj.Children)
        //            {
        //                TreeNode childNode = new TreeNode(child.Name, child.Id.ToString());
        //                selectedNode.ChildNodes.Add(childNode);
        //            }
        //        }
        //        TreeViewSystemObject.SelectedNode.Expanded = true;                
        //    }
        //}

        TreeNode selectedNode = TreeViewSystemObject.SelectedNode;
        if (selectedNode != null)
        {
            SetCurrNodeExpandState(selectedNode);
        }

        if (SelectedObjectChanged != null)
            SelectedObjectChanged(this, e);        
	}

    protected void TreeViewSystemObject_NodePopulate(Object sender, TreeNodeEventArgs e)
    {
        try
        {
            if (e.Node.ChildNodes.Count == 0)
            {
                FSEyeObject obj = AdminServer.TheInstance.SecurityManager.Get(int.Parse(e.Node.Value));
                if (obj != null)
                {
                    BuildObjectTree(e.Node, obj);
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void TreeViewSystemObject_Expanded(Object sender, TreeNodeEventArgs e)
    {
        try
        {
            if (e.Node.ChildNodes.Count == 0)
            {
                FSEyeObject obj = AdminServer.TheInstance.SecurityManager.Get(int.Parse(e.Node.Value));
                if (obj != null)
                {
                    BuildObjectTree(e.Node, obj);
                    SetCurrNodeExpandState(e.Node);
                }
            }
            //Nullable<bool> expandState = e.Node.Expanded;

            //if (expandState.HasValue && !expandState.Value)
            //{
            //    SetCurrNodeExpandState(e.Node);
            //}

            SetCurrNodeExpandState(e.Node);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void TreeViewSystemObject_Collapsed(Object sender, TreeNodeEventArgs e)
    {
        if (e.Node != null)
        {
            Nullable<bool> expandState = e.Node.Expanded;

            if (expandState.HasValue && !expandState.Value)
            {
                SetCurrNodeExpandState(e.Node);
            }
            //e.Node.Collapse();
        }
    }

    public void SetCurrNodeExpandState(TreeNode node)
    {
        TreeNode tempNode = node.Parent;
        while (tempNode != null)
        {
            tempNode.Expand();
            tempNode = tempNode.Parent;
        }
    }
}
