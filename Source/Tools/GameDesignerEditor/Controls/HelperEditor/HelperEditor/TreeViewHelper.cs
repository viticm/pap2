using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace HelperEditor
{
    public class TreeViewHelper
    {
        public static bool ClearAllEmptyLogicContents(TreeNodeCollection tnc)
        {
            bool bHasAnyRemove = false;
            foreach (TreeNode node in tnc)
            {
                // 如果还有子节点, 就开始递归
                if (node.Nodes.Count != 0)
                {
                    bHasAnyRemove = ClearAllEmptyLogicContents(node.Nodes);
                }

                // 如果自己是个非根容器, 那么
                if (node.Level > 0 && node.ForeColor != Color.Empty && node.ForeColor != Color.Black)
                {
                    // 如果是个空容器, 就删除自己
                    if (node.Nodes.Count == 0)
                    {
                        node.Remove();
                        bHasAnyRemove = true;
                    }
                    // 如果这个容器带有一个唯一的子节点, 向上一级容器合并(只有容器才能有子节点)
                    // 如果一个容器和父容器同类(同颜色), 也向上一级容器合并
                    else if (node.Nodes.Count == 1 || node.ForeColor == node.Parent.ForeColor)
                    {
                        foreach (TreeNode tempNode in node.Nodes)
                            node.Parent.Nodes.Add((TreeNode)tempNode.Clone());
                        node.Remove();      // 同时也删除自己的所有子节点
                        bHasAnyRemove = true;
                    }
                }
            }
            return bHasAnyRemove;
        }
    }
}
