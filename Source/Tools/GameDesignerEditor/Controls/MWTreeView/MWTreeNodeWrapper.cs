using System;
using System.Drawing;
using System.Windows.Forms;

/// <summary>
///	Mikael Wiberg, Copyright 2003, 2004
///		My mikwib address can be found on hotmail.com (usual HoTMaiL spam filters)
///		My mick address can be found on ar.com.au (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///		My mik address can be found on netatonce.net (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///	
///	Feel free to use the MWTreeView as you wish, as long as you do not take credit for it yourself (see next line).
///	However if it is used in commercial projects or applications it is NOT, I repeat NOT, free. Please contact me for further information.
///		Note that this differs from previous versions of the MWTreeView and I have made this decision in order to be duly compensated for my work
///			and time spent.
///	Use the MWTreeView at your own risk. I take no responsibility for ill effects happening directly or indirectly through the use of the MWTreeView.
/// </summary>
namespace MWControlSuite
{
	/// <summary>
	/// This class encapsulates a TreeNode and keeps track and deals with its ImageIndex, SelectedImageIndex, BackColor and ForeColor properties.
	/// This class was specifically created for the purpose of simulating a TreeNode being selected and deselected. The rendering of this
	///		happens somewhere inside the MS TreeView's OnPaint EventHandler - which incidentally cannot be overridden. Therefore in order
	///		to simulate the Image and color-changing parts of a TreeNode being selected the Indices have to be saved away in a class and
	///		then restored when the TreeNode is deselected.
	///	Note that as soon as an MWTreeNodeWrapper object is instantiated the TreeNode it is based on is selected. This is because the
	///		MWTreeView keeps a Hashtable of selected TreeNodes, so any MWTreeNodeWrapper object that is instantiated is assumed to be
	///		selected. This is by design.
	///	As of v1.0.5.0 the MWTreeView supports TreeNodes of any colors and the MWTreeNodeWrapper handles this.
	/// </summary>
	public class MWTreeNodeWrapper
	{
		#region Variables

		/// <summary>
		/// Core TreeNode of this MWTreeNodeWrapper object.
		/// </summary>
		private TreeNode tnNode = null;

		/// <summary>
		/// Original ImageIndex of the TreeNode in the Node property.
		/// </summary>
		private int iImageIndex = -1;

		/// <summary>
		/// Original SelectedImageIndex of the TreeNode in the Node property.
		/// </summary>
		private int iSelectedImageIndex = -1;

		/// <summary>
		/// Original BackColor of the TreeNode in the Node property.
		/// </summary>
		private Color cBackColor = SystemColors.Window;

		/// <summary>
		/// Original ForeColor of the TreeNode in the Node property.
		/// </summary>
		private Color cForeColor = SystemColors.WindowText;

		#endregion Variables



		#region Constructors

		#region Constructor for Disabling

		/// <summary>
		/// Constructor that takes a TreeNode and wraps code around it.
		/// </summary>
		/// <param name="tn">TreeNode to wrap code around.</param>
		/// <param name="cDisabledTreeNodeBackColor">BackColor to set the TreeNode supplied to in order to simulate the TreeNode being disabled.</param>
		/// <param name="cDisabledTreeNodeForeColor">ForeColor to set the TreeNode supplied to in order to simulate the TreeNode being disabled.</param>
		public MWTreeNodeWrapper(TreeNode tn, Color cDisabledTreeNodeBackColor, Color cDisabledTreeNodeForeColor)
		{
			tnNode = tn;

			cBackColor = tn.BackColor;

			cForeColor = tn.ForeColor;

			if(tn.BackColor != cDisabledTreeNodeBackColor)
			{
				tn.BackColor = cDisabledTreeNodeBackColor;
			}

			if(tn.ForeColor != cDisabledTreeNodeForeColor)
			{
				tn.ForeColor = cDisabledTreeNodeForeColor;
			}
		}

		#endregion Constructor for Disabling



		#region Constructors for Selection

		/// <summary>
		/// Constructor that takes a TreeNode and wraps code around it so as to make it appear selected.
		/// </summary>
		/// <param name="tn">TreeNode to wrap code around so as to make it appear selected.</param>
		public MWTreeNodeWrapper(TreeNode tn)
		{
			SelectionConstructorMethod(tn, true, SystemColors.Highlight, SystemColors.HighlightText);
		}

		/// <summary>
		/// Constructor that takes a TreeNode and wraps code around it so as to make it appear selected.
		/// </summary>
		/// <param name="tn">TreeNode to wrap code around so as to make it appear selected.</param>
		/// <param name="bUseExtendedSelectionColors">True if extended selection colors should be used or false otherwise.</param>
		/// <param name="cSelectedTreeNodeBackColor">If the bUseExtendedSelectionColors parameter is set to true this is the BackColor that will be used for the selected TreeNode, otherwise SystemColors.Highlight will be used.</param>
		/// <param name="cSelectedTreeNodeForeColor">If the bUseExtendedSelectionColors parameter is set to true this is the ForeColor that will be used for the selected TreeNode, otherwise SystemColors.HighlightText will be used.</param>
		public MWTreeNodeWrapper(TreeNode tn, bool bUseExtendedSelectionColors, Color cSelectedTreeNodeBackColor, Color cSelectedTreeNodeForeColor)
		{
			SelectionConstructorMethod(tn, bUseExtendedSelectionColors, cSelectedTreeNodeBackColor, cSelectedTreeNodeForeColor);
		}

		/// <summary>
		/// Method that is used by the selection constructors that takes a TreeNode and wraps code around it so as to make it appear selected.
		/// </summary>
		/// <param name="tn">TreeNode to wrap code around so as to make it appear selected.</param>
		/// <param name="bUseExtendedSelectionColors">True if extended selection colors should be used or false otherwise.</param>
		/// <param name="cSelectedTreeNodeBackColor">If the bUseExtendedSelectionColors parameter is set to true this is the BackColor that will be used for the selected TreeNode, otherwise SystemColors.Highlight will be used.</param>
		/// <param name="cSelectedTreeNodeForeColor">If the bUseExtendedSelectionColors parameter is set to true this is the ForeColor that will be used for the selected TreeNode, otherwise SystemColors.HighlightText will be used.</param>
		private void SelectionConstructorMethod(TreeNode tn, bool bUseExtendedSelectionColors, Color cSelectedTreeNodeBackColor, Color cSelectedTreeNodeForeColor)
		{
			tnNode = tn;
			iImageIndex = tnNode.ImageIndex;
			iSelectedImageIndex = tnNode.SelectedImageIndex;

			if(iImageIndex != -1)
			{
				tnNode.ImageIndex = iSelectedImageIndex;
			}
			else
			{
				tnNode.ImageIndex = tnNode.TreeView.SelectedImageIndex;
			}

			cBackColor = tn.BackColor;

			cForeColor = tn.ForeColor;

			if(tnNode.TreeView.Focused || !tnNode.TreeView.HideSelection)
			{
				if(bUseExtendedSelectionColors)
				{
					if(tn.BackColor != cSelectedTreeNodeBackColor)
					{
						tn.BackColor = cSelectedTreeNodeBackColor;
					}

					if(tn.ForeColor != cSelectedTreeNodeForeColor)
					{
						tn.ForeColor = cSelectedTreeNodeForeColor;
					}
				}
				else
				{
					if(tn.BackColor != SystemColors.Highlight)
					{
						tn.BackColor = SystemColors.Highlight;
					}

					if(tn.ForeColor != SystemColors.HighlightText)
					{
						tn.ForeColor = SystemColors.HighlightText;
					}
				}
			}
			else
			{
				if(tnNode.TreeView.Enabled)
				{
					if(tn.BackColor != tnNode.TreeView.BackColor)
					{
						tn.BackColor = tnNode.TreeView.BackColor;
					}

					if(tn.ForeColor != tnNode.TreeView.ForeColor)
					{
						tn.ForeColor = tnNode.TreeView.ForeColor;
					}
				}
			}
		}

		#endregion Constructors for Selection

		#endregion Constructors



		#region Properties

		#region Node Property

		/// <summary>
		/// Core TreeNode of this MWTreeNodeWrapper object.
		/// </summary>
		public TreeNode Node
		{
			get
			{
				return tnNode;
			}
			set
			{
				tnNode = value;
			}
		}

		#endregion Node Property



		#region ImageIndex Property

		/// <summary>
		/// Original ImageIndex of the TreeNode in the Node property.
		/// </summary>
		public int ImageIndex
		{
			get
			{
				return iImageIndex;
			}
			set
			{
				iImageIndex = value;
			}
		}

		#endregion ImageIndex Property



		#region SelectedImageIndex Property

		/// <summary>
		/// Original SelectedImageIndex of the TreeNode in the Node property.
		/// </summary>
		public int SelectedImageIndex
		{
			get
			{
				return iSelectedImageIndex;
			}
			set
			{
				iSelectedImageIndex = value;
			}
		}

		#endregion SelectedImageIndex Property



		#region BackColor Property

		/// <summary>
		/// Original BackColor of the TreeNode in the Node property.
		/// </summary>
		public Color BackColor
		{
			get
			{
				return cBackColor;
			}
			set
			{
				cBackColor = value;
			}
		}

		#endregion BackColor Property



		#region ForeColor Property

		/// <summary>
		/// Original ForeColor of the TreeNode in the Node property.
		/// </summary>
		public Color ForeColor
		{
			get
			{
				return cForeColor;
			}
			set
			{
				cForeColor = value;
			}
		}

		#endregion ForeColor Property

		#endregion Properties



		#region Methods

		#region Select Method

		/// <summary>
		/// Select a TreeNode.
		/// </summary>
		public void Select()
		{
			tnNode.ImageIndex = iSelectedImageIndex;

			cBackColor = tnNode.BackColor;
			cForeColor = tnNode.ForeColor;
		}

		#endregion Select Method



		#region Deselect Method

		/// <summary>
		/// Deselect a TreeNode.
		/// </summary>
		public void Deselect()
		{
			tnNode.ImageIndex = iImageIndex;
			tnNode.SelectedImageIndex = iSelectedImageIndex;

			if(tnNode.TreeView.Enabled)
			{
				tnNode.BackColor = cBackColor;
				tnNode.ForeColor = cForeColor;
			}
		}

		#endregion Deselect Method



		#region Enable Method

		/// <summary>
		/// Enable a TreeNode.
		/// </summary>
		public void Enable()
		{
			tnNode.BackColor = cBackColor;
			tnNode.ForeColor = cForeColor;
		}

		#endregion Enable Method

		#endregion Methods



		#region Static Methods

		#region Select Static Method

		/// <summary>
		/// Select the TreeNode in the MWTreeNodeWrapper object supplied.
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper containing TreeNode that should be selected.</param>
		public static void Select(MWTreeNodeWrapper mwtnw)
		{
			mwtnw.Node.ImageIndex = mwtnw.SelectedImageIndex;

			mwtnw.BackColor = mwtnw.Node.BackColor;
			mwtnw.ForeColor = mwtnw.Node.ForeColor;
		}

		#endregion Select Static Method



		#region Deselect Static Method

		/// <summary>
		/// Deselect the TreeNode in the MWTreeNodeWrapper object supplied.
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper containing TreeNode that should be deselected.</param>
		public static void Deselect(MWTreeNodeWrapper mwtnw)
		{
			mwtnw.Node.ImageIndex = mwtnw.ImageIndex;
			mwtnw.Node.SelectedImageIndex = mwtnw.SelectedImageIndex;

            if (mwtnw.Node.TreeView != null && mwtnw.Node.TreeView.Enabled)
			{
				mwtnw.Node.BackColor = mwtnw.BackColor;
				mwtnw.Node.ForeColor = mwtnw.ForeColor;
			}
		}

		#endregion Deselect Static Method

		#endregion Static Methods

	}

}
