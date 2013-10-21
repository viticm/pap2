using System;
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.ComponentModel.Design.Serialization;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Data;
using System.Windows.Forms;
using System.Text.RegularExpressions;

using System.Diagnostics;

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
	/// A TreeView Control that allows multiple TreeNodes to be selected at once.
	/// 
	/// Note that the MWTreeView is only using fully managed code, no interop or Win32 stuff. All code will abide by this and therefore some
	///		things are very hard to do.
	///	Any code suggestions using Win32 stuff or other is however of interest to me, but will never make it into the MWTreeView. If I receive
	///		code suggestions that are good enough and also make a real difference I might create another version of the MWTreeView, so don't
	///		hesitate to make such suggestions.
	///	Also note though that I will use the Paint methods of the original TreeView Control - I will NOT draw the whole MWTreeView myself. If
	///		that was what I wanted I would have created one from scratch, probably using less code than this one, and with more(!)
	///		functionality.
	/// 
	///	Overriding the OnBeforeSelect EventHandler is the key to having a TreeView with multi select working properly (some other people
	///		have tried, quite unsuccessfully in my view, to create multi-select TreeViews without overriding this EventHandler).
	///	The important call in the overridden OnBeforeSelect EventHandler is 'e.Cancel = true;'. This means that the MS TreeView
	///			SelectedNode property is not used. Instead the new SelNode property is used.
	///		As of v1.0.5.0 the SelectedNode property is used alongside the SelNode property for 'backwards conformity'.
	///			I suggest not to use the SelectedNode property though - use the SelNode property instead. The get accessor of the SelectedNode
	///			property is now overridden and returns the SelNode anyhow.
	/// 
	/// As of v1.0.5.0 the MWTreeView supports TreeNodes of any colors. Different colors for different TreeNodes in the same MWTreeView is also
	///		supported. And needless to say, multi selection still works.
	///	
	///	As of v2.0.0.0 the MWTreeView supports custom selection colors and it all works with HotTracking.
	/// 
	/// The AllowMultipleExpandedPaths property can be set to false which will disallow more than one branch to be open at once.
	///		Think of it as a concertina folder where you can only really have one compartment (fully) open at once. Open another one and you
	///			have to close the one that was previously open.
	/// 
	/// There are additional keyboard commands implemented for selecting/deselecting TreeNodes.
	///		Note that in order to keep the feature of scrolling the MWTreeView (inherited from MS TreeView) when the Control key is pressed
	///			while clicking the Up/Down etc keys, selecting multiple TreeNodes using the keyboard is done by holding down the Shift key. (Note
	///			the AllowModKeyShift property.)
	///		Also see the AllowControlKeyNavigation property.
	///	Holding down the Alt key selects/deselects whole branches of TreeNodes. (Note the AllowModKeyAlt property.)
	///	OnKeyDown is overridden and allows much more keyboard functinality than the original MS TreeView Control:
	///		F2			Start LabelEdit for SelNode (see the AllowKeyF2LabelEditing property).
	///		Space		Checking/Unchecking TreeNodes if the CheckBoxes property is set to true. Try holding down Control and/or Alt.
	///		Down		Moving SelNode. Try holding down Shift and/or Alt.
	///		Up			Moving SelNode. Try holding down Shift and/or Alt.
	///		Right		Moving SelNode or expanding SelNode's branch. Try holding down Shift and/or Alt.
	///		Left		Moving SelNode or collapsing SelNode's branch. Try holding down Shift and/or Alt.
	///		Home		Moving SelNode. Try holding down Shift and/or Alt.
	///		End			Moving SelNode. Try holding down Shift and/or Alt.
	///		PageUp		Moving SelNode. Try holding down Shift and/or Alt.
	///		PageDown	Moving SelNode. Try holding down Shift and/or Alt.
	///		Control-A	Selecting all TreeNodes. Try holding down Shift and/or Alt.
	///		Escape		Deselecting all TreeNodes. Try holding down Shift. (Note the AllowKeyEscapeDeselectAll property.)
	///		Control-Q	Collapse all TreeNodes. Try holding down Shift and/or Alt.
	///		Control-E	Expand all TreeNodes. Try holding down Shift and/or Alt.
	///	
	/// The MultiSelect property allows you to select TreeNodes in some various ways.
	///		Classic is the same as the normal TreeView.
	///		NoMulti works the same way as a normal MS TreeView when it comes to selecting TreeNodes - only one can be selected at a time.
	///			You do get all the other benefits of the MWTreeView even with NoMulti - mouse handling and keyboard handling etc.
	///			Note that the SelectedImageIndex DOES work when using NoMulti.
	///			Note that it is also possible not to have a TreeNode selected at all if the AllowNoSelNode property is set to true.
	///		Multi means that any TreeNodes - no restrictions - can be selected.
	///		MultiSameBranch means that the TreeNodes you select all have to be in the same branch, but CAN be on different TreeNodeLevels.
	///		MultiSameBranchAndLevel means that the TreeNodes you select all have to be in the same branch AND on the same TreeNodeLevel.
	///		MultiSameLevel means that the TreeNodes you select all have to be on the same TreeNodeLevel, but CAN be in different branches.
	///		MultiPathToParents means that the TreeNodes you select must all be in the same branch AND on the same TreeNodeLevel. All the direct
	///			Parent TreeNodes also get selected forming a sort of path to the selected TreeNodes.
	///		MultiPathToParent means that the TreeNodes you select must all be on the same TreeNodeLevel on a per root branch basis. All the
	///			direct Parent TreeNodes also get selected forming sort of paths to the selected TreeNodes.
	///		SinglePathToParent means that the TreeNode you select will also select the direct Parent TreeNodes forming a sort of path to the
	///			selected TreeNode.
	///		SinglePathToParents means that the TreeNodes you select will also select the direct Parent TreeNodes forming sort of paths to the
	///			selected TreeNodes. This is done on a per root branch basis.
	///		MultiAllDescendants is a bit like Multi with the Alt-key constantly pressed down. I.e. whichever TreeNode is selected also selects
	///			all descendants (i.e. child TreeNodes).
	///	You might wonder why anyone would want some of the options available in the MultiSelect property. Multi is pretty obvious and so is
	///			NoMulti. But the other ones... Well MultiSameBranch is the reason why I created this Control in the first place - because I
	///			needed this kind of functionality. And people have requested a couple of the other ones.
	///		So there you go.
	///	
	///	The RubberbandSelection property lets you select TreeNodes using some rubberband selection mode just like in many Windows
	///		applications - I haven't seen this in a TreeView before though (too hard? nah).
	///		The MWTreeView has support for a bunch of rubberband selection modes.
	///		Check out the new RubberbandSelectionModes!
	///		Note that the term rubberband is used very loosely here. Only two of them are, according to .NET, rubberbands (RubberbandDashed and
	///			RubberbandSolid).
	///	The rubberband selection works a bit differently than it does in e.g. the 'files panel' in Windows Explorer (which is a ListView not a
	///		TreeView). A TreeNode is selected as soon as the rubberband 'hits' the TreeNode vertically. I.e. even if the rubberband does not
	///		hit the Text of the TreeNode but e.g. a bit to the right of the Text, the TreeNode is selected. This is done because I do not have
	///		access to the ScrollBars of the TreeView and thus I cannot find out if the TreeView has been scrolled to the left a bit. If I did
	///		know this, I could easily measure the TreeNode Text and indentation (including CheckBox and Image) and find out if I hit the Text
	///		or not.
	///	Note that if the control key is pressed the rubberband method turns into a sort of painting/masking method. This means that any
	///		TreeNodes selected from the rubberband do not become unselected if the rubberband is no longer covering them. Windows Explorer
	///		toggles the entries in the 'files panel', but I don't personally like this - so I did not implement it like that intentionally.
	///	The rubberband painting can handle that the MWTreeView is scrolled vertically while painting it. Scrolling horizontally will not move
	///		the rubberband though, since again I don't have access to the ScrollBars.
	/// 
	/// The AllowBlankNodeText property allows you to set whether the Text of TreeNodes can be blank or not. If this property is set to
	///		false and there is an attempt to set the Text to string.Empty the old Text is used instead.
	///	
	///	The AllowNoSelNode property allows you to decide whether no TreeNode can be selected in the MWTreeView. If there is no TreeNode
	///		selected and this property is set to false a TreeNode is selected.
	///	
	///	If the ScrollToSelNode property is set to true the MWTreeView scrolls so that the SelNode is visible if possible.
	///	
	///	FullRowSelect has been implemented. This feature didn't seem to be 'turned on' in the MS TreeView.
	///	Because MS did such a 'good' job of hiding the ScrollBars (Controls, methods for positions etc) I couldn't find out if the Control
	///		is scrolled (if anyone has any ideas how to do this in managed, non-interop etc code, PLEASE let me know). This made it a lot
	///		harder for me to implement TreeNode selection if the click is done on something other than the Text or Image.
	/// 
	/// HotTracking has been fully implemented.
	/// 
	/// A focus rectangle is now painted around the last selected TreeNode unless the UseExtendedSelectionColors is set to true.
	///	
	///	If the LabelEditRegEx Property is set to a regular expression this has to be satisfied in order to change the TreeNode Text.
	///	Note that the LabelEditRegEx Property only applies to changes done through the GUI - NOT in code. This is intentional.
	///	
	///	If the DisallowLabelEditRegEx Property is set to a regular expression this must NOT be satisfied in order to be able to change the
	///		TreeNode Text. I.e. If you have a TreeNode whose Text currently satisfies the regular expression in the DisallowLabelEditRegEx
	///		Property, this TreeNode's Text CANNOT be changed.
	///	Note that the DisallowLabelEditRegEx Property only applies to attempted changes done through the GUI - NOT in code. This is intentional.
	/// 
	/// If the SelectNodeRegEx Property is set to a regular expression this must be satisfied in order to be able to select the TreeNode.
	/// 
	/// If the CheckNodeRegEx Property is set to a regular expression this must be satisfied in order to be able to check the TreeNode.
	/// 
	/// As of v1.0.4.0 the MWTreeView allows proper TreeNode label editing (not like previous versions where you had to double-click).
	///		Note that F2 enters label edit mode as well.
	///	
	///	Disabling the MWTreeView is now much more dynamic.
	///	
	///	See the ConstainedShiftKeySelection and ConstrainedRubberbandSelection properties for a better (default true is better) way to select
	///		TreeNodes when using MultiSameBranch, MultiSameBranchSameLevel or MultiSameLevel multi select modes.
	///	
	///	The ExtendedTemporaryMarking property has been added so that when the right mouse button is pressed on or next to the text of an unselected
	///			TreeNode it is marked as being sort of selected (image does not change, but back and fore colors change). When the right mouse
	///			button is then released the TreeNode goes back to using its own back and fore colors.
	///		The regular TreeView does this for plain-colored TreeNodes, but not for TreeNodes with a different back and/or fore color. With the
	///			ExtendedTemporaryMarking property set to true it works for non-plain-colored TreeNodes as well.
	///	
	///	The TreeNodeMouseDown property has been added to make it easier to delete a bunch of TreeNodes from a ContextMenu. (If some TreeNodes are
	///		selected and then a TreeNode that is not selected is right clicked a ContextMenu could pop up with the choice to delete TreeNodes.
	///		Expected behavior is then to have all selected TreeNodes deleted, including the one that was right clicked to get the ContextMenu up. Of
	///		course the RemoveSelectedNodes method could be called with an argument of true.)
	///	
	///	Changing ImageIndex and SelectedImageIndex of TreeNodes should be done using the ChangeTreeNodeImageIndex, ChangeTreeNodeSelectedImageIndex
	///		and ChangeTreeNodeImageIndices static methods. These methods are implemented because of the fact that MWTreeNodeWrappers are added to the
	///		SelNodes property and not normal TreeNodes. The MWTreeNodeWrappers remember which ImageIndex and SelectedImageIndex was set before it
	///		modifies it to show that a TreeNode contained in the MWTreeNodeWrapper is selected.
	///	
	///	Changing the BackColor and ForeColor of TreeNodes should be done using the ChangeBackColor, ChangeForeColor and ChangeColors methods.
	/// 
	/// Note that OnBeforeSelNodesChanged, OnAfterSelNodesChanged, OnBeforeCheckedNodesChanged and OnAfterCheckedNodesChanged events are only raised
	///		when the actual Variable underlying the Property changed - not when MWTreeNodesWrappers or TreeNodes are added or removed.
	/// 
	/// 
	/// 
	/// It may seem that there are too many options to choose from for the MWTreeView. The MWTreeView has been created so that the programmer who
	///			uses it in his or her project(s) have full(?) control over how it should behave.
	///		Many of the events being raised are quite useless in most projects, but some do need them - and that is why they are there.
	/// 
	/// 
	/// 
	/// Is there any other functionality you would like to see in the MWTreeView? Please tell me and I'll see what I can do.
	/// </summary>
	public class MWTreeView : System.Windows.Forms.TreeView
	{
		#region Variables

		#region Property Variables

		#region Browsable Property Variables

		#region Category 'Appearance, MW - Disable/Enable' Property Variables

		/// <summary>
		/// The BackColor of a Disabled TreeNode.
		/// Note that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true.
		/// </summary>
		private Color cDisabledTreeNodeBackColor = SystemColors.Control;

		/// <summary>
		/// The ForeColor of a Disabled TreeNode.
		/// Note that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true.
		/// </summary>
		private Color cDisabledTreeNodeForeColor = SystemColors.ControlDark;

		/// <summary>
		/// True if TreeNodes with other than plain colors (BackColor = SystemColors.Window and ForeColor = SystemColors.WindowText) should look properly disabled or false otherwise (standard MS TreeView appearance).
		/// </summary>
		private bool bUseExtendedDisabledColors = true;

		#endregion Category 'Appearance, MW - Disable/Enable' Property Variables



		#region Category 'Behavior, MW - Checking' Property Variables

		/// <summary>
		/// True if multiple TreeNodes can be checked at once or false otherwise (true is standard for MS TreeView).
		/// </summary>
		private bool bAllowMultiCheck = true;

		/// <summary>
		/// Regular expression that has to be satisfied before a TreeNode can be checked.
		/// </summary>
		private string strCheckNodeRegEx = string.Empty;

		#endregion Category 'Behavior, MW - Checking' Property Variables



		#region Category 'Behavior, MW - Expanding/Collapsing' Property Variables

		/// <summary>
		/// True if more than one path can be expanded at a time (standard TreeView behavior) or false otherwise.
		/// </summary>
		private bool bAllowMultipleExpandedPaths = true;

		/// <summary>
		/// True if TreeNode expands before other TreeNodes collapse or false if other TreeNodes collapse before TreeNode expands.
		/// Note that this is only used if the AllowMultipleExpandedPaths property is set to false.
		/// </summary>
		private bool bExpandBeforeCollapse = true;

		/// <summary>
		/// True if a TreeNode is collapsed when it is deselected or false otherwise.
		/// Note that this only happens if the TreeNode is expanded.
		/// </summary>
		private bool bCollapseTreeNodeOnDeselect = false;

		/// <summary>
		/// True if a TreeNode is collapsed when it is selected or false otherwise.
		/// Note that this only happens if the TreeNode is expanded.
		/// </summary>
		private bool bCollapseTreeNodeOnSelect = false;

		/// <summary>
		/// True if a TreeNode is expanded when it is deselected or false otherwise.
		/// Note that this only happens if the TreeNode is collapsed.
		/// </summary>
		private bool bExpandTreeNodeOnDeselect = false;

		/// <summary>
		/// True if a TreeNode is expanded when it is selected or false otherwise.
		/// Note that this only happens if the TreeNode is collapsed.
		/// </summary>
		private bool bExpandTreeNodeOnSelect = false;

		#endregion Category 'Behavior, MW - Expanding/Collapsing' Property Variables



		#region Category 'Behavior, MW - Keys' Property Variables

		/// <summary>
		/// True to allow the navigation/scrolling that is achieved by the normal TreeView by holding down the Control key and then pressing the Down, Up, Right, Left, Home, End, PageUp or PageDown keys or false to disallow this.
		/// </summary>
		private bool bAllowControlKeyNavigation = true;

		/// <summary>
		/// True if pressing the A-key should select all TreeNodes or false otherwise.
		/// </summary>
		private bool bAllowKeyASelectAll = true;

		/// <summary>
		/// True if pressing the E-key should expand all TreeNodes or false otherwise.
		/// </summary>
		private bool bAllowKeyEExpandAll = true;

		/// <summary>
		/// True if pressing the escape-key should deselect all selected TreeNodes or false otherwise (escape is still used for exiting LabelEdit/NodeEdit).
		/// </summary>
		private bool bAllowKeyEscapeDeselectAll = true;

		/// <summary>
		/// True if label editing starts by pressing the F2-key or false otherwise.
		/// </summary>
		private bool bAllowKeyF2LabelEditing = true;

		/// <summary>
		/// True if pressing the Q-key should collapse all TreeNodes or false otherwise.
		/// </summary>
		private bool bAllowKeyQCollapseAll = true;

		#endregion Category 'Behavior, MW - Keys' Property Variables



		#region Category 'Behavior, MW - Keys, Modifier Keys' Property Variables

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise.
		/// </summary>
		private bool bAllowKeyboardModKeyAlt = true;

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise.
		/// </summary>
		private bool bAllowKeyboardModKeyControl = true;

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise.
		/// </summary>
		private bool bAllowKeyboardModKeyShift = true;

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise.
		/// </summary>
		private bool bAllowMouseModKeyAlt = true;

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise.
		/// </summary>
		private bool bAllowMouseModKeyControl = true;

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise.
		/// </summary>
		private bool bAllowMouseModKeyShift = true;

		#endregion Category 'Behavior, MW - Keys, Modifier Keys' Property Variables



		#region Category 'Behavior, MW - Label-Editing' Property Variables

		/// <summary>
		/// True if TreeNodes can be blank, i.e. contain no Text or false otherwise.
		/// </summary>
		private bool bAllowBlankNodeText = false;

		/// <summary>
		/// Regular expression that cannot be satisfied if the Text of a TreeNode should be able to be changed.
		/// </summary>
		private string strDisallowLabelEditRegEx = string.Empty;

		/// <summary>
		/// Regular expression that has to be satisfied before the Text of a TreeNode can be changed.
		/// </summary>
		private string strLabelEditRegEx = string.Empty;

		/// <summary>
		/// Timeout (ms) used before a TreeNode is put into LabelEdit/NodeEdit mode.
		/// Default-value: 200
		/// Min-value: 0
		/// Max-value: 10000
		/// </summary>
		private int iLabelEditTimeout = 200;

		#endregion Category 'Behavior, MW - Label-Editing' Property Variables



		#region Category 'Behavior, MW - Mouse-Selection' Property Variables

		/// <summary>
		/// Decides whether clicking on a blank area should deselect all TreeNodes or not.
		/// What constitutes a blank area is decided by the MouseClearSelNodes enum.
		/// Note that some of the MouseClearSelNodes enum members or part of what they represent are ignored if the FullRowSelect property is set to true.
		/// </summary>
		private MouseClearSelNodes mcsnMouseClearSelNodes = MouseClearSelNodes.None;

		/// <summary>
		/// True if key selection is constrained by the starting TreeNode when shift-selecting or false otherwise.
		/// This is used for the following MultiSelect modes:
		///		MultiSameLevel
		///		MultiSameBranch
		///		MultiSameBranchAndLevel
		/// Tip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel.
		/// </summary>
		private bool bConstrainedShiftKeySelection = true;

		#endregion Category 'Behavior, MW - Mouse-Selection' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection' Property Variables

		/// <summary>
		/// True if rubberband selection should be allowed even though the MWTreeView, or the Form it is on, doesn't have focus or false otherwise.
		/// Note that setting this to false is probably more conventional and this is how ListViews etc work.
		/// </summary>
		private bool bAllowOutOfFocusRubberbandSelection = false;

		/// <summary>
		/// True if rubberband selection is constrained by the starting TreeNode of the rubberband selection or false otherwise.
		/// Tip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel.
		/// </summary>
		private bool bConstrainedRubberbandSelection = true;

		/// <summary>
		/// Rubberband selection mode with which TreeNodes are selected.
		/// Note that if using a MultiSelect mode of TreeViewMultiSelect.NoMulti, this property is ignored.
		/// Note that RubberbandSelectionMode.Frame will be painted even if the UseRubberbandBorder property is set to false.
		/// </summary>
		private RubberbandSelectionMode rsmRubberbandSelection = RubberbandSelectionMode.Fill;

		/// <summary>
		/// Timeout (ms) before rubberband selection is allowed after a TreeNode expand/collapse.
		/// Default-value: 1000
		/// Min-value: 0
		/// Max-value: 10000
		/// </summary>
		private int iRubberbandSelectionExpandCollapseTimeout = 1000;

		#endregion Category 'Behavior, MW - Rubberband-Selection' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Border' Property Variables

		/// <summary>
		/// Color that is used for the rubberband border.
		/// </summary>
		private Color cRubberbandBorderColor = Color.CornflowerBlue;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandBorderColor if the RubberbandBorderColor has no alpha value set.
		///		I.e. it is used when RubberbandBorderColor is 100% opaque.
		///		If RubberbandBorderColor is translucent this variable is ignored.
		/// Default-value: 255
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandBorderColorAlpha = 255;

		/// <summary>
		/// The thickness of the rubberband border.
		/// Default-value: 1
		/// Min-value: 1
		/// Max-value: 32
		/// </summary>
		private int iRubberbandBorderThickness = 1;

		/// <summary>
		/// True if a rubberband selection border should be used together with the RubberbandSelectionMode chosen.
		/// Note that this is ignored for the following RubberbandSelectionModes:
		///		RubberbandSelectionMode.None
		///		RubberbandSelectionMode.RubberbandDashed
		///		RubberbandSelectionMode.RubberbandSolid
		///		RubberbandSelectionMode.SelectionFrame
		/// </summary>
		private bool bUseRubberbandBorder = true;

		#endregion Category 'Behavior, MW - Rubberband-Selection Border' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Fill' Property Variables

		/// <summary>
		/// Color that is used for rubberband Fill.
		/// </summary>
		private Color cRubberbandFillColor = Color.CornflowerBlue;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandFillColor if the RubberbandFillColor has no alpha value set.
		///		I.e. it is used when RubberbandFillColor is 100% opaque.
		///		If RubberbandFillColor is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandFillColorAlpha = 128;

		#endregion Category 'Behavior, MW - Rubberband-Selection Fill' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Gradient' Property Variables

		/// <summary>
		/// Orientation angle that is used for rubberband Gradient.
		/// Note that this is only used if the GradientPaintMode property is set to Angle.
		/// Default-value: 45.0
		/// Min-value: -360.0
		/// Max-value: 360.0
		/// </summary>
		private float fRubberbandGradientAngle = 45.0f;

		/// <summary>
		/// Blend array that is used for rubberband Gradient.
		/// Note that this is only used when the RubberbandGradientShape property is set to GradientShape.Blend.
		/// </summary>
		private MWRubberbandGradientBlend[] amwrgbRubberbandGradientBlend = new MWRubberbandGradientBlend[0];

		/// <summary>
		/// ColorBlend array that is used for rubberband Gradient.
		/// Note that this is only used when the RubberbandGradientShape property is set to GradientShape.ColorBlend.
		/// </summary>
		private MWRubberbandGradientColorBlend[] amwrgcbRubberbandGradientColorBlend = new MWRubberbandGradientColorBlend[0];

		/// <summary>
		/// Color 1 that is used for rubberband Gradient.
		/// </summary>
		private Color cRubberbandGradientColor1 = Color.IndianRed;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandGradientColor1 if the RubberbandGradientColor1 has no alpha value set.
		///		I.e. it is used when RubberbandGradientColor1 is 100% opaque.
		///		If RubberbandGradientColor1 is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandGradientColor1Alpha = 128;

		/// <summary>
		/// Color 2 that is used for rubberband Gradient.
		/// </summary>
		private Color cRubberbandGradientColor2 = Color.CornflowerBlue;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandGradientColor2 if the RubberbandGradientColor2 has no alpha value set.
		///		I.e. it is used when RubberbandGradientColor2 is 100% opaque.
		///		If RubberbandGradientColor2 is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandGradientColor2Alpha = 128;

		/// <summary>
		/// LinearGradientMode that is used for rubberband Gradient.
		/// Note that this is only used if the GradientPaintMode property is set to LinearGradientMode.
		/// </summary>
		private LinearGradientMode lgmRubberbandGradientLinearGradientMode = LinearGradientMode.ForwardDiagonal;

		/// <summary>
		/// GradientPaintMode that is used for rubberband Gradient.
		/// </summary>
		private GradientPaintMode gpmRubberbandGradientPaintMode = GradientPaintMode.MousePoints;

		/// <summary>
		/// GradientShape that is used for rubberband Gradient.
		/// </summary>
		private GradientShape gsRubberbandGradientShape = GradientShape.Regular;

		/// <summary>
		/// Focus or center of the gradient that is used for rubberband Gradient.
		/// Default-value: 0.5
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		private float fRubberbandGradientShapeFocus = 0.5f;

		/// <summary>
		/// Scale or speed of color falloff from focus point that is used for rubberband Gradient.
		/// Default-value: 1.0
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		private float fRubberbandGradientShapeScale = 1.0f;

		#endregion Category 'Behavior, MW - Rubberband-Selection Gradient' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Hatch' Property Variables

		/// <summary>
		/// BackColor that is used for rubberband Hatch.
		/// </summary>
		private Color cRubberbandHatchBackColor = Color.IndianRed;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandHatchBackColor if the RubberbandHatchBackColor has no alpha value set.
		///		I.e. it is used when RubberbandHatchBackColor is 100% opaque.
		///		If RubberbandHatchBackColor is translucent this variable is ignored.
		/// Default-value: 64
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandHatchBackColorAlpha = 64;

		/// <summary>
		/// ForeColor that is used for rubberband Hatch.
		/// </summary>
		private Color cRubberbandHatchForeColor = Color.CornflowerBlue;

		/// <summary>
		/// Alpha channel value to be used with the RubberbandHatchForeColor if the RubberbandHatchForeColor has no alpha value set.
		///		I.e. it is used when RubberbandHatchForeColor is 100% opaque.
		///		If RubberbandHatchForeColor is translucent this variable is ignored.
		/// Default-value: 192
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		private int iRubberbandHatchForeColorAlpha = 192;

		/// <summary>
		/// HatchStyle that is used for rubberband Hatch.
		/// </summary>
		private HatchStyle hsRubberbandHatchStyle = HatchStyle.Weave;

		#endregion Category 'Behavior, MW - Rubberband-Selection Hatch' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Image' Property Variables

		/// <summary>
		/// Image that is used for rubberband Image.
		/// </summary>
		private Image imgRubberbandImage = null;

		/// <summary>
		/// True if an image that is used for rubberband Image should be scale-corrected or false otherwise.
		/// Scale-correction is needed when an image is rescaled to more than its original size and a 'whitish line' appears at the bottom and to
		///		the right of the image.
		///	Note that when setting this to true one pixel-row is lost at the bottom and one pixel-column is lost at the right of the image.
		/// </summary>
		private bool bRubberbandImageScaleCorrection = false;

		/// <summary>
		/// The transparency level for the image that is used for rubberband Image.
		/// Default-value: 0.5
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		private float fRubberbandImageTransparency = 0.5f;

		#endregion Category 'Behavior, MW - Rubberband-Selection Image' Property Variables



		#region Category 'Behavior, MW - Rubberband-Selection Selection Frame' Property Variables

		/// <summary>
		/// True if the rubberband Selection Frame should be drawn in an active state or false for a passive state.
		/// </summary>
		private bool bRubberbandSelectionFrameActive = true;

		#endregion Category 'Behavior, MW - Rubberband-Selection Selection Frame' Property Variables



		#region Category 'Behavior, MW - Rubberband Updates' Property Variables

		/// <summary>
		/// Timeout (ms) between checks to see if an update of the rubberband should be done or not.
		/// Default-value: 200
		/// Min-value: 10
		/// Max-value: 10000
		/// </summary>
		private int iRubberbandUpdateTimeout = 200;

		/// <summary>
		/// True if the OnMouseHover EventHandler should check if the rubberband needs updating or false otherwise.
		///	Note that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.
		///	Note that this can be combined with the UseRubberbandUpdateTimer property.
		/// </summary>
		private bool bUseRubberbandUpdateMouseHover = true;

		/// <summary>
		/// True if the RubberbandUpdateTimer should check if the rubberband needs updating or false otherwise.
		///	Note that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.
		///	Note that this can be combined with the UseRubberbandUpdateMouseHover property.
		/// </summary>
		private bool bUseRubberbandUpdateTimer = true;

		#endregion Category 'Behavior, MW - Rubberband Updates' Property Variables



		#region Category 'Behavior, MW - Selection' Property Variables

		/// <summary>
		/// True if no TreeNode has to be selected or false otherwise (false is standard for MS TreeView).
		/// </summary>
		private bool bAllowNoSelNode = true;

		/// <summary>
		/// True if TreeNodes with non-plain BackColor and/or ForeColor also should be temporary marked or false otherwise (standard TreeView style).
		/// Temporary marking is when a TreeNode (or next to it) is right-clicked. The TreeNode appears selected but as soon as the right mouse button
		///		is released the 'selection' is removed. For a normal TreeView this does not happen for TreeNodes with BackColor and/or ForeColor
		///		other than the standard plain ones.
		/// </summary>
		private bool bExtendedTemporaryMarking = true;

		/// <summary>
		/// Decides the multi select characteristics of an MWTreeView Control.
		/// </summary>
		private TreeViewMultiSelect tvmsMultiSelect = TreeViewMultiSelect.Multi;

		/// <summary>
		/// True if scrolling is done so the SelNode (last selected tree node) is always displayed or false otherwise.
		/// </summary>
		private bool bScrollToSelNode = true;

		/// <summary>
		/// Regular expression that has to be satisfied before a TreeNode can be selected.
		/// </summary>
		private string strSelectNodeRegEx = string.Empty;

		/// <summary>
		/// The BackColor of a selected TreeNode.
		/// Note that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true.
		/// </summary>
		private Color cSelectedTreeNodeBackColor = SystemColors.Highlight;

		/// <summary>
		/// The ForeColor of a selected TreeNode.
		/// Note that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true.
		/// </summary>
		private Color cSelectedTreeNodeForeColor = SystemColors.HighlightText;

		/// <summary>
		/// True if extended selection colors should be used for TreeNodes or false if the regular selection colors should be used.
		/// Regular BackColor is SystemColors.Highlight and regular ForeColor is SystemColors.HighlightText.
		/// </summary>
		private bool bUseExtendedSelectionColors = true;

		#endregion Category 'Behavior, MW - Selection' Property Variables

		#endregion Browsable Property Variables



		#region Non-Browsable Property Variables

		#region Category 'Behavior, MW - Checking' Property Variables

		/// <summary>
		/// HashTable containing the Checked TreeNodes as values and the TreeNode.GetHashCodes as keys.
		/// </summary>
		private Hashtable htCheckedNodes = new Hashtable();

		#endregion Category 'Behavior, MW - Checking' Property Variables



		#region Category 'Behavior, MW - Selection' Property Variables

		/// <summary>
		/// TreeNode that was in the coordinates of the OnMouseDown.
		/// Used in conjunction with rubberband selection.
		/// Note that this TreeNode is treated differently than the tnFullRowSelect TreeNode and they must therefore be separate variables.
		/// </summary>
		private TreeNode tnMouseDown = null;

		/// <summary>
		/// Last Selected TreeNode or null if no TreeNode is selected or if Last Selected TreeNode was deselected.
		/// </summary>
		private TreeNode tnSelNode = null;

		/// <summary>
		/// HashTable containing the Selected TreeNodes wrapped in MWTreeNodeWrapper objects as values and the TreeNode.GetHashCodes as keys.
		/// </summary>
		private Hashtable htSelNodes = new Hashtable();

		#endregion Category 'Behavior, MW - Selection' Property Variables

		#endregion Non-Browsable Property Variables

		#endregion Property Variables



		#region Help Variables

		#region Active/Inactive Help Variables

		/// <summary>
		/// When this MWTreeView is selected or has focus this variable is set to true.
		/// When the mouse is used to click inside this MWTreeView this variable is checked to see if the TreeNodes should be repainted.
		///		This is necessary since the MouseDown event happens before the OnGotFocus and OnEnter events.
		/// </summary>
		private bool bActive = false;

		#endregion Active/Inactive Help Variables



		#region BackColor/ForeColor for TreeNodes (Used with the ExtendedTemporaryMarking property)

		/// <summary>
		/// BackColor of TreeNode where mouse button is pressed.
		/// Used for ensuring BackColor/ForeColor of TreeNodes is correct and that the TreeNode's text gets highlighted when the mouse is pressed on it.
		/// </summary>
		private Color cBackColorTNDown = SystemColors.Window;

		/// <summary>
		/// ForeColor of TreeNode where mouse button is pressed.
		/// Used for ensuring BackColor/ForeColor of TreeNodes is correct and that the TreeNode's text gets highlighted when the mouse is pressed on it.
		/// </summary>
		private Color cForeColorTNDown = SystemColors.WindowText;

		/// <summary>
		/// OnMouseDown sets tnTNDown, OnMouseUp, OnMouseMove and OnItemDrag reset it - resetting should only happen once, and that is what this
		///		variable makes sure.
		/// </summary>
		private bool bTNDone = false;

		/// <summary>
		/// TreeNode where mouse button is pressed.
		/// Used for ensuring BackColor/ForeColor of TreeNodes is correct and that the TreeNode's text gets highlighted when the mouse is pressed on it.
		/// </summary>
		private TreeNode tnTNDown = null;

		#endregion BackColor/ForeColor for TreeNodes (Used with the ExtendedTemporaryMarking property)



		#region Checking Help Variables

		/// <summary>
		/// This variable is set to true if the AllowMultiCheck property was set to false.
		/// </summary>
		private bool bAllowMultiCheckChanged = false;

		/// <summary>
		/// This variable is set to true if the TreeNodes should be forced to be checked/unchecked.
		/// </summary>
		private bool bForceCheckNode = false;

		/// <summary>
		/// This variable is set to true if the keyboard was used for checking/unchecking a TreeNode (or group of TreeNodes).
		/// </summary>
		private bool bKeyCheck = false;

		/// <summary>
		/// This variable is set to true if the mouse was used for checking/unchecking a TreeNode (or group of TreeNodes).
		/// </summary>
		private bool bMouseCheck = false;

		#endregion Checking Help Variables



		#region Disabled TreeNodes

		/// <summary>
		/// Hashtable containing the TreeNodes that get disabled when the Enabled property gets set to false, and enabled when the Enabled property gets set to true.
		/// </summary>
		private Hashtable htDisabledTreeNodes = new Hashtable();

		#endregion Disabled TreeNodes



		#region Expand/Collapse Help Variables

		/// <summary>
		/// True if a TreeNode collapse is in progress or false otherwise.
		/// Note the RubberbandSelectionExpandCollapseTimeout that decides when rubberband selection is allowed again.
		/// </summary>
		private bool bCollapseInProgress = false;

		/// <summary>
		/// True if a TreeNode expand is in progress or false otherwise.
		/// Note the RubberbandSelectionExpandCollapseTimeout that decides when rubberband selection is allowed again.
		/// </summary>
		private bool bExpandInProgress = false;

		#endregion Expand/Collapse Help Variables



		#region Focus Rectangle and Hottracking Help Variables

		/// <summary>
		/// True if the 'next' TreeNode that enters the OnBeforeSelect EventHandler should be treated as a 'proper' selected TreeNode as far as
		///		the original TreeView Control is concerned. I.e. Hottracking works and the focus rectangle around the TreeNode is also
		///		displayed.
		///	If false the above does not happen.
		/// </summary>
		private bool bPaintFocusRectAndHottracking = false;

		#endregion Focus Rectangle and Hottracking Help Variables



		#region FullRow Help Variables

		/// <summary>
		/// TreeNode that was in the coordinates of OnMouseDown.
		/// Used in conjunction with the FullRowSelect property.
		/// Note that this TreeNode is treated differently than the tnMouseDown TreeNode and they must therefore be separate variables.
		/// </summary>
		private TreeNode tnFullRowSelect = null;

		/// <summary>
		/// The checked state of the TreeNode that was in the coordinates of OnMouseDown.
		/// Used in conjunction with the FullRowSelect property.
		/// </summary>
		private bool bFullRowSelectNodeChecked = false;

		/// <summary>
		/// The expanded state of the TreeNode that was in the coordinates of OnMouseDown.
		/// Used in conjunction with the FullRowSelect property.
		/// </summary>
		private bool bFullRowSelectNodeExpanded = false;

		/// <summary>
		/// The selected state of the TreeNode that was in the coordinates of OnMouseDown.
		/// Used in conjunction with the FullRowSelect property.
		/// </summary>
		private bool bFullRowSelectNodeSelected = false;

		#endregion FullRow Help Variables



		#region LabelEdit Help Variables

		/// <summary>
		/// TreeNode that is used by the tLabelEdit_Tick EventHandler. This is set in BeginEditMethod.
		/// </summary>
		private TreeNode tnLabelEdit = null;

		/// <summary>
		/// True if a Label is allowed to be edited.
		/// Note that this.LabelEdit has to be true as well.
		/// </summary>
		private bool bLabelEditAllowed = false;

		#endregion LabelEdit Help Variables



		#region MouseDown, MouseMove and Rubberband Help Variables

		/// <summary>
		/// Point in the coordinates of the OnMouseDown.
		/// Used in conjunction with rubberband selection.
		/// Note that this variable changes if the Control is scrolled vertically while a rubberband is painted.
		/// Also see variable ptMouseDownOrig.
		/// </summary>
		private Point ptMouseDown = new Point(0, 0);

		/// <summary>
		/// Point in the coordinates of the OnMouseDown.
		/// Used in conjunction with rubberband selection.
		/// Note that this variable does NOT change if the Control is scrolled vertically while a rubberband is painted - this is the original
		///		position from the OnMouseDown EventHandler and therefore does not change until the next OnMouseDown call.
		/// Also see variable ptMouseDown.
		/// </summary>
		private Point ptMouseDownOrig = new Point(0, 0);

		/// <summary>
		/// Point in the coordinates of the OnMouseDown transformed to screen coordinates.
		/// Used in conjunction with rubberband selection.
		/// </summary>
		private Point ptMouseDownScreen = new Point(0, 0);

		/// <summary>
		/// Point in the coordinates of the OnMouseDown transformed to client coordinates.
		/// Used in conjunction with rubberband selection.
		/// </summary>
		private Point ptMouseDownClient = new Point(0, 0);

		/// <summary>
		/// Point in the coordinates of the mouse position in the OnMouseMove EventHandler transformed to screen coordinates.
		/// Used in conjunction with rubberband selection.
		/// </summary>
		private Point ptMouseMoveScreen = new Point(0, 0);

		/// <summary>
		/// Point in the coordinates of the mouse position in the OnMouseMove EventHandler transformed to client coordinates.
		/// Used in conjunction with rubberband selection.
		/// </summary>
		private Point ptMouseMoveClient = new Point(0, 0);

		/// <summary>
		/// True if a rubberband has been painted (and therefore must be cleared) or false otherwise.
		/// Used in conjunction with rubberband selection.
		/// </summary>
		private bool bRubberbandHasBeenPainted = false;

		/// <summary>
		/// True if rubberband is on screen (or will be as soon as the method in which it is set is finished) or false otherwise.
		/// This variable helps in determining whether or not to start LabelEditing from mouse clicks (OnMouseUp).
		/// </summary>
		private bool bRubberbandIsOnScreen = false;

		#endregion MouseDown, MouseMove and Rubberband Help Variables



		#region Updated, RubberbandStartMoved and RubberbandArea

		/// <summary>
		/// Bool that is used in OnMouseMove and the newed method Update.
		/// It is used to reduce flickering when holding the rubberband still (while the rubberband is on-screen).
		/// Used together with the bRubberbandStartMoved variable.
		/// </summary>
		private bool bUpdated = false;

		/// <summary>
		/// Bool that is used in OnMouseMove and MoveRubberbandStart.
		/// It is used to reduce flickering when holding the rubberband still (while the rubberband is on-screen).
		/// Used together with the bUpdate variable.
		/// </summary>
		private bool bRubberbandStartMoved = false;

		/// <summary>
		/// This variable is used when creating the MWRubberbandClearEventArgs in the following methods:
		///		OnMouseUp
		///		OnMouseMove
		///		OnMouseLeave
		/// </summary>
		private Rectangle rctRubberbandArea = Rectangle.Empty;

		#endregion Updated, RubberbandStartMoved and RubberbandArea

		#endregion Help Variables



		#region Component Designer generated Variables

		private System.Windows.Forms.Timer tLabelEdit;
		private System.Windows.Forms.Timer tLabelEditAllowed;
		private System.Windows.Forms.Timer tRubberbandSelectionAllowedCollapse;
		private System.Windows.Forms.Timer tRubberbandSelectionAllowedExpand;
		private System.Windows.Forms.Timer tRubberbandUpdate;

		private System.ComponentModel.IContainer components;

		#endregion Component Designer generated Variables

		#endregion Variables



		#region Constructor & Dispose

		/// <summary>
		/// Standard constructor.
		/// </summary>
		public MWTreeView()
		{
			//Set a few ControlStyles (note that not all are used/necessary, AllPaintingInWmPaint, DoubleBuffer and UserPaint are though).

			this.SetStyle(System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);
			this.SetStyle(System.Windows.Forms.ControlStyles.DoubleBuffer, true);
			this.SetStyle(System.Windows.Forms.ControlStyles.ResizeRedraw, true);
			this.SetStyle(System.Windows.Forms.ControlStyles.Selectable, true);
			this.SetStyle(System.Windows.Forms.ControlStyles.SupportsTransparentBackColor, true);
			//this.SetStyle(System.Windows.Forms.ControlStyles.UserPaint, true);

			InitializeComponent();

			tLabelEdit.Interval = iLabelEditTimeout;
			tLabelEditAllowed.Interval = iLabelEditTimeout;
			tRubberbandSelectionAllowedCollapse.Interval = iRubberbandSelectionExpandCollapseTimeout;
			tRubberbandSelectionAllowedExpand.Interval = iRubberbandSelectionExpandCollapseTimeout;
			tRubberbandUpdate.Interval = iRubberbandUpdateTimeout;
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if(disposing)
			{
				if(components != null)
				{
					components.Dispose();
				}
			}

			base.Dispose(disposing);
		}

		#endregion Constructor & Dispose



		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.tLabelEdit = new System.Windows.Forms.Timer(this.components);
			this.tLabelEditAllowed = new System.Windows.Forms.Timer(this.components);
			this.tRubberbandSelectionAllowedExpand = new System.Windows.Forms.Timer(this.components);
			this.tRubberbandSelectionAllowedCollapse = new System.Windows.Forms.Timer(this.components);
			this.tRubberbandUpdate = new System.Windows.Forms.Timer(this.components);
			// 
			// tLabelEdit
			// 
			this.tLabelEdit.Interval = 200;
			this.tLabelEdit.Tick += new System.EventHandler(this.tLabelEdit_Tick);
			// 
			// tLabelEditAllowed
			// 
			this.tLabelEditAllowed.Interval = 200;
			this.tLabelEditAllowed.Tick += new System.EventHandler(this.tLabelEditAllowed_Tick);
			// 
			// tRubberbandSelectionAllowedExpand
			// 
			this.tRubberbandSelectionAllowedExpand.Interval = 1000;
			this.tRubberbandSelectionAllowedExpand.Tick += new System.EventHandler(this.tRubberbandSelectionAllowedExpand_Tick);
			// 
			// tRubberbandSelectionAllowedCollapse
			// 
			this.tRubberbandSelectionAllowedCollapse.Interval = 1000;
			this.tRubberbandSelectionAllowedCollapse.Tick += new System.EventHandler(this.tRubberbandSelectionAllowedCollapse_Tick);
			// 
			// tRubberbandUpdate
			// 
			this.tRubberbandUpdate.Interval = 200;
			this.tRubberbandUpdate.Tick += new System.EventHandler(this.tRubberbandUpdate_Tick);

		}

		#endregion Component Designer generated code



		#region Overridden and Newed Properties, EventHandlers and Methods

		#region Overridden EventHandlers

		#region Checking (OnBeforeCheck, OnBeforeCheckMethod & OnAfterCheck)

		#region OnBeforeCheck

		/// <summary>
		/// Standard OnBeforeCheck EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewCancelEventArgs object.</param>
		protected override void OnBeforeCheck(System.Windows.Forms.TreeViewCancelEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(e.Action == TreeViewAction.Unknown && !bKeyCheck && !bMouseCheck && !bAllowMultiCheckChanged && !bForceCheckNode)
				{
					e.Cancel = true;
				}

				if(e.Action == TreeViewAction.ByMouse)
				{
					bMouseCheck = true;

					BeforeCheckMethod(e.Node, e.Action != TreeViewAction.ByKeyboard);

					bLabelEditAllowed = false;

					if(this.AllowMultiCheck)
					{
						if(IsTreeNodeChecked(e.Node))
						{
							//Opposite, since the TreeNode hasn't been unchecked yet.
							if(e.Node.Checked)
							{
								this.CheckedNodesRemove(e.Node);
							}
						}
						else
						{
							if(this.CheckedNodes == null)
							{
								this.CheckedNodes = new Hashtable();
							}

							//Opposite, since the TreeNode hasn't been checked yet.
							if(!e.Node.Checked && IsCheckNodeRegExSatisfied(e.Node.Text))
							{
								this.CheckedNodesAdd(e.Node);
							}
						}
					}
					else
					{
						//Opposite, since the TreeNode hasn't been unchecked yet.
						if(e.Node.Checked)
						{
							this.CheckedNodesRemove(e.Node);
						}
						else if(IsCheckNodeRegExSatisfied(e.Node.Text))
						{
							if(this.CheckedNodes == null)
							{
								this.CheckedNodes = new Hashtable();
							}

							this.ClearCheckedNodes();

							this.CheckedNodesAdd(e.Node);
						}
					}

					bMouseCheck = false;
				}

				if(!bMouseCheck)
				{
					if(!IsCheckNodeRegExSatisfied(e.Node.Text) && !e.Node.Checked)
					{
						e.Cancel = true;
					}

					base.OnBeforeCheck(e);
				}
			}
			else
			{
				base.OnBeforeCheck(e);
			}
		}

		#endregion OnBeforeCheck



		#region BeforeCheckMethod

		/// <summary>
		/// Main method used when checking/unchecking TreeNodes using the mouse.
		/// </summary>
		/// <param name="tnMain">Node to be checked/unchecked.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		private void BeforeCheckMethod(TreeNode tn, bool bUpdate)
		{
			if(	MouseModKeyShift ||
				MouseModKeyControl ||
				MouseModKeyAlt)
			{
				bAllowMultiCheckChanged = true;

				if(tn.Parent != null)
				{
					if(	!MouseModKeyShift &&
						!MouseModKeyControl &&
						!MouseModKeyAlt)
					{
						ToggleCheckNode(tn.Parent, bUpdate);
					}

					if(MouseModKeyControl)
					{
						ToggleCheckBranch(tn.Parent, true, tn, bUpdate);
					}
					else if(MouseModKeyShift && MouseModKeyAlt)
					{
						ToggleCheckBranch(tn.Parent, false, tn, bUpdate);
						ToggleCheckBranch(tn, true, bUpdate);
					}
					else if(MouseModKeyShift)
					{
						ToggleCheckBranch(tn.Parent, false, tn, bUpdate);
					}
					else if(MouseModKeyAlt)
					{
						ToggleCheckBranch(tn, true, bUpdate);
					}
				}
				else
				{
					if(MouseModKeyControl)
					{
						foreach(TreeNode tn2 in this.Nodes)
						{
							if(tn != tn2)
							{
								ToggleCheckNode(tn2, bUpdate);
							}

							ToggleCheckBranch(tn2, true, tn, bUpdate);
						}
					}
					else if(MouseModKeyShift && MouseModKeyAlt)
					{
						foreach(TreeNode tn2 in this.Nodes)
						{
							if(tn != tn2)
							{
								ToggleCheckNode(tn2, bUpdate);
							}
						}

						ToggleCheckBranch(tn, true, bUpdate);
					}
					else if(MouseModKeyShift)
					{
						foreach(TreeNode tn2 in this.Nodes)
						{
							if(tn != tn2)
							{
								ToggleCheckNode(tn2, bUpdate);
							}
						}
					}
					else if(MouseModKeyAlt)
					{
						ToggleCheckBranch(tn, true, bUpdate);
					}
				}

				bAllowMultiCheckChanged = false;
			}
		}

		#endregion BeforeCheckMethod



		#region OnAfterCheck

		/// <summary>
		/// Standard OnAfterCheck EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewEventArgs object.</param>
		protected override void OnAfterCheck(TreeViewEventArgs e)
		{
			bLabelEditAllowed = false;

			base.OnAfterCheck(e);

			BeginAllowLabelEditing();
		}

		#endregion OnAfterCheck

		#endregion Checking (OnBeforeCheck & OnBeforeCheckMethod & OnAfterCheck)



		#region Clicking (OnDoubleClick)

		/// <summary>
		/// Standard OnDoubleClick EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnDoubleClick(EventArgs e)
		{
			bLabelEditAllowed = false;

			base.OnDoubleClick(e);

			BeginAllowLabelEditing();
		}

		#endregion Clicking (OnDoubleClick)



		#region Expanding/Collapsing (OnBeforeExpand/OnAfterExpand & OnBeforeCollapse/OnAfterCollapse)

		#region Expanding (OnBeforeExpand/OnAfterExpand)

		/// <summary>
		/// Standard OnBeforeExpand EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewCancelEventArgs object.</param>
		protected override void OnBeforeExpand(TreeViewCancelEventArgs e)
		{
			bExpandInProgress = true;

			base.OnBeforeExpand(e);

			if(this.ExpandBeforeCollapse)
			{
				if(!this.AllowMultipleExpandedPaths)
				{
					TreeNode tnGrandParent = GetTreeNodeGrandParent(e.Node);

					foreach(TreeNode tn in this.Nodes)
					{
						if(tn != tnGrandParent)
						{
							tn.Collapse();
						}
					}
				}
			}
		}

		/// <summary>
		/// Standard OnAfterExpand EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewEventArgs object.</param>
		protected override void OnAfterExpand(TreeViewEventArgs e)
		{
			base.OnAfterExpand(e);

			if(!this.ExpandBeforeCollapse)
			{
				if(!this.AllowMultipleExpandedPaths)
				{
					TreeNode tnGrandParent = GetTreeNodeGrandParent(e.Node);

					foreach(TreeNode tn in this.Nodes)
					{
						if(tn != tnGrandParent)
						{
							tn.Collapse();
						}
					}
				}
			}

			BeginAllowRubberbandSelectionExpand();
		}

		#endregion Expanding (OnBeforeExpand/OnAfterExpand)



		#region Collapsing (OnBeforeCollapse/OnAfterCollapse)

		/// <summary>
		/// Standard OnBeforeCollapse EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewCancelEventArgs object.</param>
		protected override void OnBeforeCollapse(TreeViewCancelEventArgs e)
		{
			bCollapseInProgress = true;

			base.OnBeforeCollapse(e);
		}

		/// <summary>
		/// Standard OnAfterCollapse EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewEventArgs object.</param>
		protected override void OnAfterCollapse(TreeViewEventArgs e)
		{
			base.OnAfterCollapse(e);

			BeginAllowRubberbandSelectionCollapse();
		}

		#endregion Collapsing (OnBeforeCollapse/OnAfterCollapse)

		#endregion Expanding/Collapsing (OnBeforeExpand/OnAfterExpand & OnBeforeCollapse/OnAfterCollapse)



		#region Focus and Activation (OnGotFocus/OnLostFocus & OnEnter/OnLeave)

		#region Focus(OnGotFocus/OnLostFocus)

		/// <summary>
		/// Standard OnGotFocus EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnGotFocus(System.EventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bLabelEditAllowed = false;

				bActive = true;

				ActivateSelNodes();
			}

			base.OnGotFocus(e);

			BeginAllowLabelEditing();
		}

		/// <summary>
		/// Standard OnLostFocus EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnLostFocus(System.EventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bLabelEditAllowed = false;

				bActive = false;

				DeactivateSelNodes();
			}

			base.OnLostFocus(e);
		}

		#endregion Focus (OnGotFocus/OnLostFocus)



		#region Activation (OnEnter/OnLeave)

		/// <summary>
		/// Standard OnEnter EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnEnter(System.EventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bLabelEditAllowed = false;

				bActive = true;

				ActivateSelNodes();
			}

			base.OnEnter(e);

			BeginAllowLabelEditing();
		}

		/// <summary>
		/// Standard OnLeave EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnLeave(System.EventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bLabelEditAllowed = false;

				bActive = false;

				DeactivateSelNodes();
			}

			base.OnLeave(e);
		}

		#endregion Activation (OnEnter/OnLeave)

		#endregion Focus and Activation (OnGotFocus/OnLostFocus & OnEnter/OnLeave)



		#region OnItemDrag

		/// <summary>
		/// Standard OnItemDrag EventHandler.
		/// </summary>
		/// <param name="e">Standard ItemDragEventArgs object.</param>
		protected override void OnItemDrag(ItemDragEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				tRubberbandUpdate.Stop();

				if(this.ExtendedTemporaryMarking && !bTNDone && tnTNDown != null && !IsTreeNodeSelected(tnTNDown))
				{
					bTNDone = true;
					tnTNDown.BackColor = cBackColorTNDown;
					tnTNDown.ForeColor = cForeColorTNDown;
				}
			}

			base.OnItemDrag(e);
		}

		#endregion OnItemDrag



		#region Keys (OnKeyDown)

		/// <summary>
		/// Standard OnKeyDown EventHandler.
		/// </summary>
		/// <param name="e">Standard KeyEventArgs object.</param>
		protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
		{
			//Note that '!KeyboardModKeyNavigation(e.Modifiers)' is used instead of '!KeyboardModKeyControl(e.Modifiers)' for the following keys:
			//	Keys.Down
			//	Keys.Up
			//	Keys.Right
			//	Keys.Left
			//	Keys.Home
			//	Keys.End
			//	Keys.PageUp
			//	Keys.PageDown
			//This is intentional! This is to separate the navigation/scrolling of the MWTreeView from other commands that rely on pressing the Control key.

			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bool bThisSelNodeIsNull = false;

				switch(e.KeyCode)
				{
						#region Down Key (Move, Scroll and/or Select)

					case Keys.Down:
						if(this.SelNode != null && this.SelNode.NextVisibleNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							TreeNode tnTemp = this.SelNode;

							if(!KeyboardModKeyShift(e.Modifiers))
							{
								ClearSelNodes();
							}

							if(!KeyboardModKeyAlt(e.Modifiers))
							{
								SelectNode(tnTemp.NextVisibleNode, true);
							}
							else
							{
								TreeNode tnTempNextVisibleNode = tnTemp.NextVisibleNode;
								if(KeyboardModKeyShift(e.Modifiers))
								{
									SelectBranch(tnTemp, true, false, true);
								}
								SelectBranch(tnTempNextVisibleNode, true, false, true);
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion Down Key (Move, Scroll and/or Select)



						#region Up Key (Move, Scroll and/or Select)

					case Keys.Up:
						if(this.SelNode != null && this.SelNode.PrevVisibleNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							TreeNode tnTemp = this.SelNode;

							if(!KeyboardModKeyShift(e.Modifiers))
							{
								ClearSelNodes();
							}

							if(!KeyboardModKeyAlt(e.Modifiers))
							{
								SelectNode(tnTemp.PrevVisibleNode, true);
							}
							else
							{
								TreeNode tnTempPrevVisibleNode = tnTemp.PrevVisibleNode;
								if(KeyboardModKeyShift(e.Modifiers))
								{
									SelectBranch(tnTemp, true, false, true);
								}
								SelectBranch(tnTempPrevVisibleNode, true, true, true);
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion Up Key (Move, Scroll and/or Select)



						#region Right Key (Move, Scroll and/or Select)

					case Keys.Right:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							if(!this.SelNode.IsExpanded)
							{
								this.SelNode.Expand();

								if(!KeyboardModKeyShift(e.Modifiers))
								{
									ClearSelNodes();
								}

								SelectNode(this.SelNode, true);
							}
							else
							{
								if(!KeyboardModKeyShift(e.Modifiers))
								{
									ClearSelNodes();
								}

								SelectNode(this.SelNode.FirstNode, true);
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion Right Key (Move, Scroll and/or Select)



						#region Left Key (Move, Scroll and/or Select)

					case Keys.Left:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							if(this.SelNode.IsExpanded)
							{
								this.SelNode.Collapse();

								if(!KeyboardModKeyShift(e.Modifiers))
								{
									ClearSelNodes();
								}

								SelectNode(this.SelNode, true);
							}
							else
							{
								if(this.SelNode.Parent != null)
								{
									if(!KeyboardModKeyShift(e.Modifiers))
									{
										ClearSelNodes();
									}

									SelectNode(this.SelNode.Parent, true);
								}
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion Left Key (Move, Scroll and/or Select)



						#region Home Key (Move, Scroll and/or Select)

					case Keys.Home:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							if(!KeyboardModKeyShift(e.Modifiers) && !KeyboardModKeyAlt(e.Modifiers))
							{
								ClearSelNodes();

								SelectNode(this.Nodes[0], true);
							}
							else
							{
								if(!KeyboardModKeyAlt(e.Modifiers))
								{
									TreeNode tn = this.SelNode;
									while(tn != null && tn.PrevVisibleNode != null)
									{
										tn = tn.PrevVisibleNode;
										SelectNode(tn, true);
									}
								}
								else
								{
									TreeNode tn = this.SelNode;

									if(KeyboardModKeyShift(e.Modifiers))
									{
										SelectBranch(tn, true, true, true);
									}

									while(tn != null && tn.PrevVisibleNode != null)
									{
										tn = tn.PrevVisibleNode;
										SelectBranch(tn, true, true, true);
									}
								}
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion Home Key (Move, Scroll and/or Select)



						#region End Key (Move, Scroll and/or Select)

					case Keys.End:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							if(!KeyboardModKeyShift(e.Modifiers) && !KeyboardModKeyAlt(e.Modifiers))
							{
								ClearSelNodes();

								TreeNode tn = this.SelNode;
								while(tn != null && tn.NextVisibleNode != null)
								{
									tn = tn.NextVisibleNode;
								}
								SelectNode(tn, true);
							}
							else
							{
								if(!KeyboardModKeyAlt(e.Modifiers))
								{
									TreeNode tn = this.SelNode;
									while(tn != null && tn.NextVisibleNode != null)
									{
										tn = tn.NextVisibleNode;
										SelectNode(tn, true);
									}
								}
								else
								{
									TreeNode tn = this.SelNode;

									if(KeyboardModKeyShift(e.Modifiers))
									{
										SelectBranch(tn, true, false, true);
									}

									while(tn != null && tn.NextVisibleNode != null)
									{
										tn = tn.NextVisibleNode;
										SelectBranch(tn, true, false, true);
									}
								}
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion End Key (Move, Scroll and/or Select)



						#region PageUp Key (Move, Scroll and/or Select)

					case Keys.PageUp:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							TreeNode tnMax = this.GetNodeAt(0, 0);
							TreeNode tnTemp = this.SelNode;

							if(!KeyboardModKeyShift(e.Modifiers))
							{
								ClearSelNodes();
								DeselectNode(this.SelNode, true);
								this.SelNode = null;

								if(!KeyboardModKeyAlt(e.Modifiers))
								{
									bThisSelNodeIsNull = true;
								}
							}

							if(tnMax != this.SelNode && !bThisSelNodeIsNull)
							{
								if(tnMax != null)
								{
									TreeNode tn = tnTemp;

									if(KeyboardModKeyAlt(e.Modifiers) && KeyboardModKeyShift(e.Modifiers))
									{
										SelectBranch(tn, true, true, true);
									}
									else
									{
										SelectNode(tn, true);
									}

									while(tn != null && tn.PrevVisibleNode != null && tn != tnMax)
									{
										tn = tn.PrevVisibleNode;

										if(KeyboardModKeyAlt(e.Modifiers))
										{
											SelectBranch(tn, true, true, true);
										}
										else if(KeyboardModKeyShift(e.Modifiers))
										{
											SelectNode(tn, true);
										}
									}

									if(KeyboardModKeyAlt(e.Modifiers))
									{
										SelectBranch(tnMax, true, true, true);
									}
									else
									{
										SelectNode(tnMax, true);
									}
								}
							}
							else
							{
								TreeNode tn = tnTemp;
								int iMax = this.ClientRectangle.Height / this.ItemHeight;
								for(int i = 0; i < iMax; i++)
								{
									if(tn.PrevVisibleNode != null)
									{
										if(KeyboardModKeyAlt(e.Modifiers))
										{
											SelectBranch(tn, true, true, true);
										}
										else if(KeyboardModKeyShift(e.Modifiers))
										{
											SelectNode(tn, true);
										}
										tn = tn.PrevVisibleNode;
									}
									else
									{
										break;
									}
								}

								if(KeyboardModKeyAlt(e.Modifiers))
								{
									SelectBranch(tnMax, true, true, true);
								}
								else
								{
									SelectNode(tn, true);
								}
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion PageUp Key (Move, Scroll and/or Select)



						#region PageDown Key (Move, Scroll and/or Select)

					case Keys.PageDown:
						if(this.SelNode != null && !KeyboardModKeyNavigation(e.Modifiers))
						{
							TreeNode tnMax = this.GetNodeAt(0, this.ClientRectangle.Height - this.ItemHeight);
							TreeNode tnTemp = this.SelNode;

							if(!KeyboardModKeyShift(e.Modifiers))
							{
								ClearSelNodes();
								DeselectNode(this.SelNode, true);
								this.SelNode = null;

								if(!KeyboardModKeyAlt(e.Modifiers))
								{
									bThisSelNodeIsNull = true;
								}
							}

							if(tnMax == null)
							{
								SelectNode(tnMax, true);

								tnMax = this.Nodes[this.Nodes.Count - 1];
								while(tnMax != null && tnMax.NextVisibleNode != null)
								{
									tnMax = tnMax.NextVisibleNode;
								}
							}

							if(tnMax != this.SelNode && !bThisSelNodeIsNull)
							{
								if(tnMax != null)
								{
									TreeNode tn = tnTemp;

									if(KeyboardModKeyAlt(e.Modifiers) && KeyboardModKeyShift(e.Modifiers))
									{
										SelectBranch(tn, true, false, true);
									}
									else
									{
										SelectNode(tn, true);
									}

									while(tn != null && tn.NextVisibleNode != null && tn != tnMax)
									{
										tn = tn.NextVisibleNode;

										if(KeyboardModKeyAlt(e.Modifiers))
										{
											SelectBranch(tn, true, false, true);
										}
										else if(KeyboardModKeyShift(e.Modifiers))
										{
											SelectNode(tn, true);
										}
									}

									if(KeyboardModKeyAlt(e.Modifiers))
									{
										SelectBranch(tn, true, false, true);
									}
									else
									{
										SelectNode(tnMax, true);
									}
								}
							}
							else
							{
								TreeNode tn = tnTemp;
								int iMax = this.ClientRectangle.Height / this.ItemHeight;
								for(int i = 0; i < iMax; i++)
								{
									if(tn.NextVisibleNode != null)
									{
										if(KeyboardModKeyAlt(e.Modifiers))
										{
											SelectBranch(tn, true, false, true);
										}
										else if(KeyboardModKeyShift(e.Modifiers))
										{
											SelectNode(tn, true);
										}
										tn = tn.NextVisibleNode;
									}
									else
									{
										break;
									}
								}

								if(KeyboardModKeyAlt(e.Modifiers))
								{
									SelectBranch(tn, true, false, true);
								}
								else
								{
									SelectNode(tn, true);
								}
							}

							if(KeyboardModKeyControl(e.Modifiers))
							{
								e.Handled = true;
							}
						}

						break;

						#endregion PageDown Key (Move, Scroll and/or Select)



						#region Space Key (Checking)

					case Keys.Space:
						if(this.CheckBoxes)
						{
							bKeyCheck = true;

							if((KeyboardModKeyShift(e.Modifiers) || KeyboardModKeyControl(e.Modifiers)) && this.AllowMultiCheck)
							{
								if(this.SelectedNode != null)
								{
									if(this.SelectedNode.Parent != null)
									{
										if(!(KeyboardModKeyShift(e.Modifiers) && KeyboardModKeyControl(e.Modifiers)))
										{
											ToggleCheckNode(this.SelectedNode.Parent, true);
										}

										if(KeyboardModKeyControl(e.Modifiers))
										{
											ToggleCheckBranch(this.SelectedNode.Parent, true, this.SelectedNode, true);
											ToggleCheckNode(this.SelectedNode, false);
										}
										else
										{
											ToggleCheckBranch(this.SelectedNode.Parent, false, this.SelectedNode, true);
											ToggleCheckNode(this.SelectedNode, false);
										}
									}
									else
									{
										if(KeyboardModKeyControl(e.Modifiers))
										{
											foreach(TreeNode tn in this.Nodes)
											{
												if(tn != this.SelectedNode)
												{
													ToggleCheckNode(tn, true);
												}

												ToggleCheckBranch(tn, true, tn, true);
											}
										}
										else
										{
											foreach(TreeNode tn in this.Nodes)
											{
												if(tn != this.SelectedNode)
												{
													ToggleCheckNode(tn, true);
												}
											}
										}
									}
								}
							}
							else if(this.SelectedNode != null)
							{
								ToggleCheckNode(this.SelectedNode, false);
							}

							bKeyCheck = false;
						}

						break;

						#endregion Space Key (Checking)



						#region A Key (Selecting)

					case Keys.A:
						if(this.AllowKeyASelectAll)
						{
							if(this.MultiSelect != TreeViewMultiSelect.NoMulti && KeyboardModKeyControl(e.Modifiers))
							{
								switch(this.MultiSelect)
								{
									case TreeViewMultiSelect.Multi:
									case TreeViewMultiSelect.MultiAllDescendants:
										if(KeyboardModKeyAlt(e.Modifiers) && KeyboardModKeyShift(e.Modifiers) && this.SelNode != null)
										{
											if(this.SelNode.Parent != null)
											{
												SelectBranch(this.SelNode, true, true, true);

												SelectAllParentNodes(this.SelNode, false);
											}
											else
											{
												SelectBranch(this.SelNode, true, true, true);
											}
										}
										else if(KeyboardModKeyAlt(e.Modifiers) && this.SelNode != null)
										{
											if(this.SelNode.Parent != null)
											{
												TreeNode tnTemp = this.SelNode;

												foreach(TreeNode tn in this.SelNode.Parent.Nodes)
												{
													SelectBranch(tn, true, true, true);
												}

												SelectNode(tnTemp, true);
											}
											else
											{
												TreeNode tnTemp = this.SelNode;

												foreach(TreeNode tn in this.Nodes)
												{
													SelectBranch(tn, true, true, true);
												}

												SelectNode(tnTemp, true);
											}
										}
										else if(KeyboardModKeyShift(e.Modifiers) && this.SelNode != null)
										{
											if(this.SelNode.Parent != null)
											{
												TreeNode tnTemp = this.SelNode;

												foreach(TreeNode tn in this.SelNode.Parent.Nodes)
												{
													SelectNode(tn, true);
												}

												SelectNode(tnTemp, true);
											}
											else
											{
												TreeNode tnTemp = this.SelNode;

												foreach(TreeNode tn in this.Nodes)
												{
													SelectNode(tn, true);
												}

												SelectNode(tnTemp, true);
											}
										}
										else
										{
											ClearSelNodes();

											SelectAllNodes();
										}
										break;

									case TreeViewMultiSelect.MultiSameBranchAndLevel:
										if(this.SelNode.Parent != null)
										{
											foreach(TreeNode tn in this.SelNode.Parent.Nodes)
											{
												SelectNode(tn, false);
											}
										}
										break;

									case TreeViewMultiSelect.MultiSameBranch:
										if(this.SelNode.Parent != null)
										{
											if(KeyboardModKeyShift(e.Modifiers) && KeyboardModKeyAlt(e.Modifiers))
											{
												SelectBranch(this.SelNode, true, true, true);

												SelectAllParentNodes(this.SelNode, false);
											}
											else if(KeyboardModKeyShift(e.Modifiers) || KeyboardModKeyAlt(e.Modifiers))
											{
												if(KeyboardModKeyShift(e.Modifiers))
												{
													foreach(TreeNode tn in this.SelNode.Parent.Nodes)
													{
														SelectNode(tn, false);
													}
												}
												else if(KeyboardModKeyAlt(e.Modifiers))
												{
													foreach(TreeNode tn in this.SelNode.Parent.Nodes)
													{
														SelectBranch(tn, true, true, true);
													}
												}
											}
											else
											{
												SelectBranch(GetTreeNodeGrandParent(this.SelNode), true, true, true);
											}
										}
										else
										{
											if(!KeyboardModKeyShift(e.Modifiers) || (KeyboardModKeyShift(e.Modifiers) && KeyboardModKeyAlt(e.Modifiers)))
											{
												SelectBranch(this.SelNode, true, true, true);
											}
										}
										break;

									case TreeViewMultiSelect.MultiSameLevel:
										if(this.SelNode.Parent != null)
										{
											if(KeyboardModKeyShift(e.Modifiers))
											{
												foreach(TreeNode tn in this.SelNode.Parent.Nodes)
												{
													SelectNode(tn, false);
												}
											}
											else
											{
												SelectAllNodes(GetTreeNodeLevel(this.SelNode), false);
											}
										}
										else
										{
											foreach(TreeNode tn in this.Nodes)
											{
												SelectNode(tn, false);
											}
										}
										break;

									case TreeViewMultiSelect.MultiPathToParents:
									case TreeViewMultiSelect.MultiPathToParent:
										if(this.SelNode != null)
										{
											SelectAllParentNodes(this.SelNode, false);

											if(this.SelNode.Parent != null)
											{
												foreach(TreeNode tn in this.SelNode.Parent.Nodes)
												{
													SelectNode(tn, false);
												}
											}
											else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParents)
											{
												foreach(TreeNode tn in this.Nodes)
												{
													SelectNode(tn, false);
												}
											}
										}
										else
										{
											if(this.Nodes.Count > 0)
											{
												SelectBranch(this.Nodes[0], true, true, true);
											}
										}
										break;

									case TreeViewMultiSelect.SinglePathToParent:
										break;

								}
							}
						}

						break;

						#endregion A Key (Selecting)



						#region E Key (Expanding)

					case Keys.E:
						if(this.AllowKeyEExpandAll)
						{
							if(KeyboardModKeyControl(e.Modifiers))
							{
								if(KeyboardModKeyAlt(e.Modifiers) && this.SelNode != null)
								{
									this.SelNode.ExpandAll();
								}
								else if(KeyboardModKeyShift(e.Modifiers) && this.SelNode != null)
								{
									this.SelNode.Expand();
								}
								else
								{
									this.ExpandAll();
								}
							}
						}

						break;

						#endregion E Key (Expanding)



						#region Escape Key (Deselecting)

					case Keys.Escape:
						if(this.AllowKeyEscapeDeselectAll)
						{
							if(KeyboardModKeyShift(e.Modifiers) && this.SelNode != null)
							{
								if(this.SelNode != null)
								{
									if(this.SelNode.Parent != null)
									{
										DeselectBranch(this.SelNode.Parent, true, true);
									}
									else
									{
										foreach(TreeNode tn in this.Nodes)
										{
											DeselectBranch(tn, true, true);
										}
									}
								}
							}
							else
							{
								this.SelNode = null;
								ClearSelNodes();
							}
						}

						break;

						#endregion Escape Key (Deselecting)



						#region F2 Key (Label-Editing)

					case Keys.F2:
						if(this.AllowKeyF2LabelEditing)
						{
							if(	this.LabelEdit &&
								(!bRubberbandHasBeenPainted ||
								bRubberbandHasBeenPainted && MWTreeView.MouseButtons == MouseButtons.None))
							{
								if(this.HotTracking)
								{
									if(base.SelectedNode != null)
									{
										base.SelectedNode.BeginEdit();
									}
									else if(this.SelNodes.Count > 0)
									{
										foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
										{
											mwtnw.Node.BeginEdit();
											break;
										}
									}
								}
								else
								{
									if(this.SelNode != null)
									{
										this.SelNode.BeginEdit();
									}
									else if(this.SelNodes.Count > 0)
									{
										foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
										{
											mwtnw.Node.BeginEdit();
											break;
										}
									}
								}
							}
						}

						break;

						#endregion F2 Key (Label-Editing)



						#region Q Key (Collapsing)

					case Keys.Q:
						if(this.AllowKeyQCollapseAll)
						{
							if(KeyboardModKeyControl(e.Modifiers))
							{
								if(KeyboardModKeyAlt(e.Modifiers) && this.SelNode != null)
								{
									this.SelNode.Collapse();
								}
								else if(KeyboardModKeyShift(e.Modifiers) && this.SelNode != null)
								{
									this.SelNode.Collapse();
								}
								else
								{
									this.CollapseAll();
								}
							}
						}

						break;

						#endregion Q Key (Collapsing)

				}

				EnsureSelNodeNotNull();
			}

			base.OnKeyDown(e);
		}

		#endregion Keys (OnKeyDown)



		#region Label Editing (OnBeforeLabelEdit/OnAfterLabelEdit)

		/// <summary>
		/// Standard OnBeforeLabelEdit EventHandler.
		/// </summary>
		/// <param name="e">Standard NodeLabelEditEventArgs object.</param>
		protected override void OnBeforeLabelEdit(System.Windows.Forms.NodeLabelEditEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(IsDisallowLabelEditRegExSatisfied(e.Node.Text))
				{
					e.CancelEdit = true;
				}
			}

			base.OnBeforeLabelEdit(e);
		}

		/// <summary>
		/// Standard OnAfterLabelEdit EventHandler.
		/// </summary>
		/// <param name="e">Standard NodeLabelEditEventArgs object.</param>
		protected override void OnAfterLabelEdit(System.Windows.Forms.NodeLabelEditEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				bLabelEditAllowed = false;

				if(	(this.AllowBlankNodeText && e.Label == string.Empty) ||
					((e.Label != null && e.Label != string.Empty) &&
					IsLabelEditRegExSatisfied(e.Label)))
				{
					base.OnAfterLabelEdit(e);
				}
				else
				{
					e.CancelEdit = true;
				}
			}
			else
			{
				base.OnAfterLabelEdit(e);
			}
		}

		#endregion Label Editing (OnBeforeLabelEdit/OnAfterLabelEdit)



		#region Mouse (OnMouseDown, OnMouseUp, OnMouseMove, MoveRubberbandStart & OnMouseLeave)

		#region OnMouseDown

		/// <summary>
		/// Standard OnMouseDown EventHandler.
		/// </summary>
		/// <param name="e">Standard MouseEventArgs object.</param>
		protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				this.SelectedNode = null;

				tRubberbandSelectionAllowedExpand.Stop();
				tRubberbandSelectionAllowedCollapse.Stop();
				bExpandInProgress = false;
				bCollapseInProgress = false;

				if(!bActive)
				{
					this.Focus();
					ActivateSelNodes();
					bActive = true;
				}

				this.TreeNodeMouseDownInt = this.GetNodeAt(e.X, e.Y);
				ptMouseDown = new Point(e.X, e.Y);
				ptMouseDownOrig = new Point(e.X, e.Y);

				if(this.TreeNodeMouseDownInt == null && this.Nodes.Count > 0)
				{
					this.TreeNodeMouseDownInt = this.Nodes[this.Nodes.Count - 1];

					while(this.TreeNodeMouseDownInt.NextVisibleNode != null)
					{
						this.TreeNodeMouseDownInt = this.TreeNodeMouseDownInt.NextVisibleNode;
					}
				}

				TreeNode tn = this.GetNodeAt(e.X, e.Y);
				TreeNode tnSel = this.SelNode;

				if(tn != null)
				{
					tnTNDown = tn;
					cBackColorTNDown = tnTNDown.BackColor;
					cForeColorTNDown = tnTNDown.ForeColor;

					if(e.Button == MouseButtons.Left)
					{
						if(	this.FullRowSelect &&
							!bExpandInProgress && !bCollapseInProgress)
						{
							tnFullRowSelect = tn;
							bFullRowSelectNodeSelected = IsTreeNodeSelected(tn);
							bFullRowSelectNodeChecked = tn.Checked;
							bFullRowSelectNodeExpanded = tn.IsExpanded;
						}
						else
						{
							tnFullRowSelect = null;
						}
					}

					base.OnMouseDown(e);

					if(this.ExtendedTemporaryMarking && e.Button == MouseButtons.Right && tnTNDown != null && !IsTreeNodeSelected(tnTNDown))
					{
						tnTNDown.BackColor = this.SelectedTreeNodeBackColor;
						tnTNDown.ForeColor = this.SelectedTreeNodeForeColor;
						bTNDone = false;
					}
				}
				else
				{
					base.OnMouseDown(e);
				}
			}
			else
			{
				base.OnMouseDown(e);
			}
		}

		#endregion OnMouseDown



		#region OnMouseUp

		/// <summary>
		/// Standard OnMouseUp EventHandler.
		/// </summary>
		/// <param name="e">Standard MouseEventArgs object.</param>
		protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
		{
			//Nice little way of finding out exactly where the mouse was released.
			//TreeNode tnDebug = this.GetNodeAt(e.X, e.Y);
			//if(tnDebug != null)
			//{
			//	Debug.WriteLine("------------------------------- " + tnDebug.Text);
			//	Debug.WriteLine("IsPointOnTreeNode             : " + IsPointOnTreeNode(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeText         : " + IsPointOnTreeNodeText(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeTextExact    : " + IsPointOnTreeNodeTextExact(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeRow          : " + IsPointOnTreeNodeRow(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeImage        : " + IsPointOnTreeNodeImage(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeImageExact   : " + IsPointOnTreeNodeImageExact(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeCheckBox     : " + IsPointOnTreeNodeCheckBox(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointOnTreeNodeCheckBoxExact: " + IsPointOnTreeNodeCheckBoxExact(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointLeftOfTreeNode         : " + IsPointLeftOfTreeNode(tnDebug, ptMouseDown));
			//	Debug.WriteLine("IsPointRightOfTreeNode        : " + IsPointRightOfTreeNode(tnDebug, ptMouseDown));
			//	Debug.WriteLine("-------------------------------------");
			//}
			//else
			//{
			//	Debug.WriteLine("TreeNode is NULL");
			//}
 
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(this.ExtendedTemporaryMarking && !bTNDone && tnTNDown != null && !IsTreeNodeSelected(tnTNDown))
				{
					bTNDone = true;
					tnTNDown.BackColor = cBackColorTNDown;
					tnTNDown.ForeColor = cForeColorTNDown;
				}

				TreeNode tn = this.GetNodeAt(e.X, e.Y);


				if(	this.ConstrainedRubberbandSelection &&
					(
					this.MultiSelect == TreeViewMultiSelect.MultiSameLevel ||
					this.MultiSelect == TreeViewMultiSelect.MultiSameBranch ||
					this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel
					) &&
					tn != null &&
					tn != tnMouseDown &&
					!IsTreeNodeSelected(tn))
				{
					TreeNode tnTemp = tnMouseDown;

					bool bPrevious = false;

					while(tnTemp != null && tnTemp.PrevVisibleNode != null && tnTemp != tn)
					{
						if(tnTemp.PrevVisibleNode == tn)
						{
							bPrevious = true;
							break;
						}

						tnTemp = tnTemp.PrevVisibleNode;
					}

					if(bPrevious)
					{
						tnTemp = tn;

						while(!IsTreeNodeSelected(tnTemp))
						{
							if(tnTemp.NextVisibleNode != null)
							{
								tnTemp = tnTemp.NextVisibleNode;
							}
							else
							{
								break;
							}
						}

						tn = tnTemp;
					}
					else
					{
						tnTemp = tn;

						while(!IsTreeNodeSelected(tnTemp))
						{
							if(tnTemp.PrevVisibleNode != null)
							{
								tnTemp = tnTemp.PrevVisibleNode;
							}
							else
							{
								break;
							}
						}

						tn = tnTemp;
					}
				}


				if(bRubberbandIsOnScreen && bRubberbandHasBeenPainted)
				{
					switch(this.RubberbandSelection)
					{
						case RubberbandSelectionMode.RubberbandDashed:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Dashed);
							break;

						case RubberbandSelectionMode.RubberbandSolid:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Thick);
							break;

						case RubberbandSelectionMode.SelectionFrame:
						case RubberbandSelectionMode.Frame:
						case RubberbandSelectionMode.Fill:
						case RubberbandSelectionMode.Hatch:
							//case RubberbandSelectionMode.Texture:
						case RubberbandSelectionMode.Gradient:
							//case RubberbandSelectionMode.PathGradient:
						case RubberbandSelectionMode.Image:
							this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
							this.Update();
							break;

						case RubberbandSelectionMode.Custom:
							MWRubberbandClearEventArgs ce = new MWRubberbandClearEventArgs(GetPositiveRectangle(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y)), rctRubberbandArea);

							try
							{
								OnRubberbandClear(ce);

								this.Invalidate(ce.ClearArea, false);
								this.Update();
							}
							catch
							{
								this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
								this.Update();
							}

							break;

						default:
							//RubberbandSelectionMode.None ends up here
							break;
					}

					tRubberbandUpdate.Stop();

					SelectNode(tn, true);

					BeginAllowLabelEditing();
				}
				else
				{
					if(tnFullRowSelect != null)
					{
						if(	this.FullRowSelect &&
							bFullRowSelectNodeSelected == IsTreeNodeSelected(tnFullRowSelect) &&
							bFullRowSelectNodeChecked == tnFullRowSelect.Checked &&
							bFullRowSelectNodeExpanded == tnFullRowSelect.IsExpanded &&
							!bExpandInProgress && !bCollapseInProgress)
						{
							BeforeSelectMethod(tnFullRowSelect);
						}
					}
				}

				if(	e.Button == MouseButtons.Left &&
					!MouseModKeyAlt &&
					!MouseModKeyControl &&
					!MouseModKeyShift &&
					AllowMouseClearSelNodes(tn, new Point(e.X, e.Y)))
				{
					if(	(bRubberbandIsOnScreen && bRubberbandHasBeenPainted && tnMouseDown == null) ||
						(!(bRubberbandIsOnScreen && bRubberbandHasBeenPainted)))
					{
						if(this.AllowNoSelNode)
						{
							this.SelNodeInt = null;
							this.ClearSelNodes();
						}
						else
						{
							this.ClearSelNodes(this.SelNode);
						}
					}
				}

				tnFullRowSelect = null;

				bRubberbandIsOnScreen = false;
				bRubberbandHasBeenPainted = false;
			}

			base.OnMouseUp(e);
		}

		#endregion OnMouseUp



		#region OnMouseMove

		/// <summary>
		/// Standard OnMouseMove EventHandler.
		/// </summary>
		/// <param name="e">Standard MouseEventArgs object.</param>
		protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(this.ExtendedTemporaryMarking && !bTNDone && tnTNDown != null && !IsTreeNodeSelected(tnTNDown))
				{
					bTNDone = true;
					tnTNDown.BackColor = cBackColorTNDown;
					tnTNDown.ForeColor = cForeColorTNDown;
				}

				int iMouseDownTreeNodeLevel = GetTreeNodeLevel(this.TreeNodeMouseDownInt);
				TreeNode tnMouseDownGrandParent = GetTreeNodeGrandParent(this.TreeNodeMouseDownInt);

				bool bStartRubberbandPainting = Math.Abs(ptMouseDown.Y - e.Y) > this.ItemHeight;

				if(bStartRubberbandPainting)
				{
					if(this.UseRubberbandUpdateTimer)
					{
						tRubberbandUpdate.Start();
					}

					bRubberbandIsOnScreen = true;
				}

				TreeNode tn = this.GetNodeAt(e.X, e.Y);

				if(this.HotTracking)
				{
					if(IsTreeNodeSelected(tn) && IsPointOnTreeNode(tn, new Point(e.X, e.Y)))
					{
						bPaintFocusRectAndHottracking = true;
						this.SelectedNode = tn;
						bPaintFocusRectAndHottracking = false;
					}
					else
					{
						this.SelectedNode = null;
					}
				}

				if(	((this.AllowOutOfFocusRubberbandSelection && !bActive) || bActive) &&
					RubberbandSelection != RubberbandSelectionMode.None)
				{
					if(	e.Button == MouseButtons.Left &&
						this.MultiSelect != TreeViewMultiSelect.NoMulti &&
						(bStartRubberbandPainting || bRubberbandHasBeenPainted || (tn != this.TreeNodeMouseDownInt && tn != null)) &&
						!bExpandInProgress &&
						!bCollapseInProgress &&
						IsMouseMoveSelect)
					{
						if(!(ptMouseDownScreen == this.PointToScreen(ptMouseDown) &&
							ptMouseMoveScreen == this.PointToScreen(new Point(e.X, e.Y))) ||
							bUpdated || bRubberbandStartMoved)
						{

							#region OnMouseMove Selection

							if(this.TreeNodeMouseDownInt != null && tn != null)
							{
								TreeNode tnTemp = this.TreeNodeMouseDownInt;

								if(tnTemp != tn)
								{
									bool bPrevious = false;

									while(tnTemp != null && tnTemp.PrevVisibleNode != null && tnTemp != tn)
									{
										if(tnTemp.PrevVisibleNode == tn)
										{
											bPrevious = true;
											break;
										}

										tnTemp = tnTemp.PrevVisibleNode;
									}

									tnTemp = this.TreeNodeMouseDownInt;

									if(!MouseModKeyControl)
									{
										if(bPrevious)
										{
											this.ClearSelNodes(tn, this.TreeNodeMouseDownInt);
										}
										else
										{
											this.ClearSelNodes(this.TreeNodeMouseDownInt, tn);
										}
									}

									if(	MouseModKeyAlt &&
										this.MultiSelect != TreeViewMultiSelect.MultiSameLevel &&
										this.MultiSelect != TreeViewMultiSelect.MultiPathToParent &&
										this.MultiSelect != TreeViewMultiSelect.MultiPathToParents &&
										this.MultiSelect != TreeViewMultiSelect.SinglePathToParent &&
										this.MultiSelect != TreeViewMultiSelect.SinglePathToParents &&
										this.MultiSelect != TreeViewMultiSelect.MultiAllDescendants)
									{
										SelectBranch(this.TreeNodeMouseDownInt, true, true, false);
									}
									else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
									{
										SelectBranch(this.TreeNodeMouseDownInt, true, true, false);
									}
									else
									{
										SelectNode(this.TreeNodeMouseDownInt, false);
									}

									if(bPrevious)
									{
										while(tnTemp.PrevVisibleNode != null && tnTemp != tn)
										{
											if(	this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
											{
												if(iMouseDownTreeNodeLevel == GetTreeNodeLevel(tnTemp.PrevVisibleNode))
												{
													SelectNode(tnTemp.PrevVisibleNode, false);
												}
											}
											else if(this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
											{
												if(tnMouseDownGrandParent == GetTreeNodeGrandParent(tnTemp.PrevVisibleNode))
												{
													SelectNode(tnTemp.PrevVisibleNode, false);
												}
											}
											else if(this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
											{
												if(	iMouseDownTreeNodeLevel == GetTreeNodeLevel(tnTemp.PrevVisibleNode) &&
													tnMouseDownGrandParent == GetTreeNodeGrandParent(tnTemp.PrevVisibleNode))
												{
													SelectNode(tnTemp.PrevVisibleNode, false);
												}
											}
											else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
												this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
												this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
												this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
											{
												SelectNode(tnTemp.PrevVisibleNode, false);
											}
											else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
											{
												if(MouseModKeyAlt)
												{
													SelectNode(tnTemp.PrevVisibleNode, false);
												}
												else
												{
													SelectBranch(tnTemp.PrevVisibleNode, true, false, false);
												}
											}
											else
											{
												if(MouseModKeyAlt)
												{
													SelectBranch(tnTemp.PrevVisibleNode, true, false, false);
												}
												else
												{
													SelectNode(tnTemp.PrevVisibleNode, false);
												}
											}

											tnTemp = tnTemp.PrevVisibleNode;
										}
									}
									else
									{
										while(tnTemp.NextVisibleNode != null && tnTemp != tn)
										{
											if(	this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
											{
												if(iMouseDownTreeNodeLevel == GetTreeNodeLevel(tnTemp.NextVisibleNode))
												{
													SelectNode(tnTemp.NextVisibleNode, false);
												}
											}
											else if(this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
											{
												if(tnMouseDownGrandParent == GetTreeNodeGrandParent(tnTemp.NextVisibleNode))
												{
													SelectNode(tnTemp.NextVisibleNode, false);
												}
											}
											else if(this.ConstrainedRubberbandSelection &&
												this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
											{
												if(	iMouseDownTreeNodeLevel == GetTreeNodeLevel(tnTemp.NextVisibleNode) &&
													tnMouseDownGrandParent == GetTreeNodeGrandParent(tnTemp.NextVisibleNode))
												{
													SelectNode(tnTemp.NextVisibleNode, false);
												}
											}
											else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
												this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
												this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
												this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
											{
												SelectNode(tnTemp.NextVisibleNode, false);
											}
											else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
											{
												if(MouseModKeyAlt)
												{
													SelectNode(tnTemp.NextVisibleNode, false);
												}
												else
												{
													SelectBranch(tnTemp.NextVisibleNode, true, true, false);
												}
											}
											else
											{
												if(MouseModKeyAlt)
												{
													SelectBranch(tnTemp.NextVisibleNode, true, true, false);
												}
												else
												{
													SelectNode(tnTemp.NextVisibleNode, false);
												}
											}

											tnTemp = tnTemp.NextVisibleNode;
										}
									}
								}
								else
								{
									if(!MouseModKeyControl)
									{
										ClearSelNodes(tn);
									}
								}

								if(	this.ConstrainedRubberbandSelection &&
									this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
								{
									if(iMouseDownTreeNodeLevel == GetTreeNodeLevel(tn))
									{
										SelectNode(tn, false);
									}
								}
								else if(this.ConstrainedRubberbandSelection &&
									this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
								{
									if(tnMouseDownGrandParent == GetTreeNodeGrandParent(tn))
									{
										SelectNode(tn, false);
									}
								}
								else if(this.ConstrainedRubberbandSelection &&
									this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
								{
									if(	iMouseDownTreeNodeLevel == GetTreeNodeLevel(tn) &&
										tnMouseDownGrandParent == GetTreeNodeGrandParent(tn))
									{
										SelectNode(tn, false);
									}
								}
								else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
									this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
									this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
									this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
								{
									SelectNode(tn, false);
								}
								else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
								{
									if(MouseModKeyAlt)
									{
										SelectNode(tn, false);
									}
									else
									{
										SelectBranch(tn, true, true, false);
									}
								}
								else
								{
									if(MouseModKeyAlt)
									{
										SelectBranch(tn, true, true, false);
									}
									else
									{
										SelectNode(tn, false);
									}
								}
							}

							#endregion OnMouseMove Selection


							MoveRubberbandStart();

							Point ptMouseMoveClientInt = this.PointToClient(ptMouseMoveScreen);

							//The next line is not used because if the Control is scrolled horizontally while the rubberband is painted it is not cleared properly.
							//Rectangle rctInvalidate1 = new Rectangle(ptMouseDown.X, ptMouseDown.Y, ptMouseMoveClientInt.X - ptMouseDown.X, ptMouseMoveClientInt.Y - ptMouseDown.Y);
							Rectangle rctInvalidate1 = new Rectangle(this.ClientRectangle.Left, ptMouseDown.Y, this.ClientRectangle.Width, ptMouseMoveClientInt.Y - ptMouseDown.Y);

							//Since calling ControlPaint.DrawReversibleFrame with FrameStyle.Thick seems to 'bleed' over the top edge by one
							//	pixel the following is necessary.
							if(this.RubberbandSelection == RubberbandSelectionMode.RubberbandSolid)
							{
								rctInvalidate1 = GetPositiveRectangle(rctInvalidate1);
								rctInvalidate1 = new Rectangle(rctInvalidate1.X, rctInvalidate1.Y - 1, rctInvalidate1.Width, rctInvalidate1.Height + 1);
							}

							ptMouseDownScreen = this.PointToScreen(ptMouseDown);
							ptMouseMoveScreen = this.PointToScreen(new Point(e.X, e.Y));
							ptMouseDownClient = ptMouseDown;
							ptMouseMoveClient = new Point(e.X, e.Y);

							Rectangle rctSelection = new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y);
							Rectangle rctSelectionClient = new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y);
							Rectangle rctPositiveSelectionClient = GetPositiveRectangle(rctSelectionClient);
							Rectangle rctPositiveSelectionClientFrame = GetPositiveRectangleSmaller(rctSelectionClient, this.RubberbandBorderThickness);
							Rectangle rctPositiveSelectionClientInside = GetPositiveRectangleInside(rctPositiveSelectionClient, this.RubberbandBorderThickness);

							//This is used in OnMouseUp, OnMouseMove, OnMouseLeave to give more accurate feedback to the OnRubberbandClear EventHandler.
							rctRubberbandArea = rctPositiveSelectionClient;

							//The next line is not used because if the Control is scrolled horizontally while the rubberband is painted it is not cleared properly.
							//Rectangle rctInvalidate2 = new Rectangle(ptMouseDown.X, ptMouseDown.Y, e.X - ptMouseDown.X, e.Y - ptMouseDown.Y);
							Rectangle rctInvalidate2 = new Rectangle(this.ClientRectangle.Left, ptMouseDown.Y, this.ClientRectangle.Width, e.Y - ptMouseDown.Y);

							//Invalidate is now done further down.
							//Two Rectangles need to be Invalidated because the new Rectangle could be bigger than the old one or smaller.
							//this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
							//this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);

							//Update is now done in the switch-statement below so that the RubberbandSelectionModes that invalidate more
							//	than rctInvalidate1 and rctInvalidate2 do not cause Update to be called twice.
							//this.Update(false);


							#region RubberbandSelection Painting

							Graphics g = this.CreateGraphics();

							//When Rectangle is zero-width or zero-height it cannot be painted (that is why there are try-catch-statements below).
							switch(this.RubberbandSelection)
							{

									#region RubberbandDashed

								case RubberbandSelectionMode.RubberbandDashed:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Update(false);

									ControlPaint.DrawReversibleFrame(rctSelection, this.BackColor, FrameStyle.Dashed);

									break;
									#endregion RubberbandDashed



									#region RubberbandSolid

								case RubberbandSelectionMode.RubberbandSolid:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Update(false);

									ControlPaint.DrawReversibleFrame(rctSelection, this.BackColor, FrameStyle.Thick);

									break;

									#endregion RubberbandSolid



									#region SelectionFrame

								case RubberbandSelectionMode.SelectionFrame:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Invalidate(rctPositiveSelectionClient, false);
									this.Update(false);

									try
									{
										Rectangle rctPositiveSelectionClientInsideTemp = rctPositiveSelectionClientInside;

										//Make sure the RubberbandBorderThickness property is used as thickness and not just to not draw in a Rectangle.
										if(	rctPositiveSelectionClient.Width - 2 * this.RubberbandBorderThickness < 0 ||
											rctPositiveSelectionClient.Height - 2 * this.RubberbandBorderThickness < 0)
										{
											rctPositiveSelectionClientInsideTemp = new Rectangle(0, 0, 0, 0);
										}

										ControlPaint.DrawSelectionFrame(g, this.RubberbandSelectionFrameActive, rctPositiveSelectionClient, rctPositiveSelectionClientInsideTemp, this.BackColor);
									}
									catch
									{
									}

									break;

									#endregion SelectionFrame



									#region Frame

								case RubberbandSelectionMode.Frame:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Invalidate(rctPositiveSelectionClient, false);
									this.Update(false);

									try
									{
										g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
									}
									catch
									{
									}

									break;

									#endregion Frame



									#region Fill

								case RubberbandSelectionMode.Fill:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Invalidate(rctPositiveSelectionClient, false);
									this.Update(false);

									try
									{
										if(this.UseRubberbandBorder)
										{
											g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
										}
									}
									catch
									{
									}

									try
									{
										if(this.UseRubberbandBorder)
										{
											g.FillRectangle(new SolidBrush(this.RubberbandFillColor), rctPositiveSelectionClientInside);
										}
										else
										{
											g.FillRectangle(new SolidBrush(this.RubberbandFillColor), rctPositiveSelectionClient);
										}
									}
									catch
									{
									}

									break;

									#endregion Fill



									#region Hatch

								case RubberbandSelectionMode.Hatch:
									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Invalidate(rctPositiveSelectionClient, false);
									this.Update(false);

									try
									{
										if(this.UseRubberbandBorder)
										{
											g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
										}
									}
									catch
									{
									}

									try
									{
										if(this.UseRubberbandBorder)
										{
											g.FillRectangle(new HatchBrush(this.RubberbandHatchStyle, this.RubberbandHatchForeColor, this.RubberbandHatchBackColor), rctPositiveSelectionClientInside);
										}
										else
										{
											g.FillRectangle(new HatchBrush(this.RubberbandHatchStyle, this.RubberbandHatchForeColor, this.RubberbandHatchBackColor), rctPositiveSelectionClient);
										}
									}
									catch
									{
									}

									//Here is an example of painting everything to an image and then updating and displaying the image.
									//	I experienced no speed increase what-so-ever using this method.
									//
									//Rectangle rctPositiveSelectionClientInsideOrigo = new Rectangle(Point.Empty, rctPositiveSelectionClientInside.Size - new Size(2, 2));
									//Rectangle rctPositiveSelectionClientFrameOrigo = new Rectangle(Point.Empty, rctPositiveSelectionClientFrame.Size - new Size(2, 2));
									//
									//try
									//{
									//	Bitmap bmp = new Bitmap(rctPositiveSelectionClientInside.Width, rctPositiveSelectionClientInside.Height);
									//	Image img = (Image)bmp;
									//	Graphics gnew = Graphics.FromImage(img);
									//	gnew.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrameOrigo);
									//	gnew.FillRectangle(new HatchBrush(this.RubberbandHatchStyle, this.RubberbandHatchForeColor, this.RubberbandHatchBackColor), rctPositiveSelectionClientInsideOrigo);
									//	gnew.Dispose();
									//
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									//	this.Invalidate(rctPositiveSelectionClient, false);
									//	this.Update(false);
									//
									//	g.DrawImage(img, rctPositiveSelectionClient, 0, 0, img.Width, img.Height, GraphicsUnit.Pixel);
									//}
									//catch(Exception ex)
									//{
									//	int a = 0;
									//}

									break;

									#endregion Hatch



									#region Texture

									//case RubberbandSelectionMode.Texture:
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									//	this.Invalidate(rctPositiveSelectionClient, false);
									//	this.Update(false);
									//	//NOT IMPLEMENTED YET.
									//
									//	try
									//	{
									//		if(this.UseRubberbandBorder)
									//		{
									//			g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
									//		}
									//	}
									//	catch
									//	{
									//	}
									//
									//	try
									//	{
									//		//DRAW USING TEXTURE BRUSH.
									//
									//		Image imgTexture = Image.FromFile("background.jpg");
									//		//Image imgTexture = Image.FromFile("test_001a.png");
									//		//Image imgTexture = Image.FromFile("background_32x32.png");
									//
									//		//g.FillRectangle(new TextureBrush(img, rct, ImageAttributes), rct);
									//		//g.FillRectangle(new TextureBrush(img, rct), rct);
									//		//g.FillRectangle(new TextureBrush(img, WrapMode, rct), rct);
									//		//g.FillRectangle(new TextureBrush(img, WrapMode), rct);
									//		//g.FillRectangle(new TextureBrush(img), rct);
									//
									//		ImageAttributes iaTexture = new ImageAttributes();
									//		ColorMatrix cm = new ColorMatrix();
									//		cm.Matrix00 = 1.0f;
									//		cm.Matrix01 = 0.0f;
									//		cm.Matrix02 = 0.0f;
									//		cm.Matrix03 = 0.0f;
									//		cm.Matrix04 = 0.0f;
									//		cm.Matrix10 = 0.0f;
									//		cm.Matrix11 = 1.0f;
									//		cm.Matrix12 = 0.0f;
									//		cm.Matrix13 = 0.0f;
									//		cm.Matrix14 = 0.0f;
									//		cm.Matrix20 = 0.0f;
									//		cm.Matrix21 = 0.0f;
									//		cm.Matrix22 = 1.0f;
									//		cm.Matrix23 = 0.0f;
									//		cm.Matrix24 = 0.0f;
									//		cm.Matrix30 = 0.0f;
									//		cm.Matrix31 = 0.0f;
									//		cm.Matrix32 = 0.0f;
									//		cm.Matrix33 = 0.5f;
									//		cm.Matrix34 = 0.0f;
									//		cm.Matrix40 = 0.0f;
									//		cm.Matrix41 = 0.0f;
									//		cm.Matrix42 = 0.0f;
									//		cm.Matrix43 = 0.0f;
									//		cm.Matrix44 = 1.0f;
									//		iaTexture.SetColorMatrix(cm);
									//
									//		//Convert to grayscale:
									//		//cm = new ColorMatrix(new float[][]{	new float[]{0.3f,0.3f,0.3f,0,0},
									//		//										new float[]{0.59f,0.59f,0.59f,0,0},
									//		//										new float[]{0.11f,0.11f,0.11f,0,0},
									//		//										new float[]{0,0,0,1,0,0},
									//		//										new float[]{0,0,0,0,1,0},
									//		//										new float[]{0,0,0,0,0,1}});
									//		//iaTexture.SetColorMatrix(cm);
									//
									//		//iaTexture.SetWrapMode(WrapMode.Tile);
									//		g.FillRectangle(new TextureBrush(imgTexture, rctPositiveSelectionClientInside, iaTexture), rctPositiveSelectionClientInside);
									//		//g.DrawImage(imgTexture, rctPositiveSelectionClientInside, 0, 0, imgTexture.Width, imgTexture.Height, GraphicsUnit.Pixel, iaTexture);
									//		//g.FillRectangle(new TextureBrush(imgTexture), rctPositiveSelectionClientInside);
									//	}
									//	catch(Exception ex)
									//	{
									//		//Out of memory.
									//		int a = 0;
									//	}
									//
									//	break;

									#endregion Texture



									#region Gradient

								case RubberbandSelectionMode.Gradient:
									try
									{
										LinearGradientBrush lgb = null;
										Rectangle rctGradient = Rectangle.Empty;

										if(this.RubberbandGradientPaintMode == GradientPaintMode.MousePoints)
										{
											if(this.UseRubberbandBorder)
											{
												lgb = new LinearGradientBrush(new Point(ptMouseDown.X - this.RubberbandBorderThickness, ptMouseDown.Y - this.RubberbandBorderThickness), new Point(e.X - this.RubberbandBorderThickness, e.Y - this.RubberbandBorderThickness), this.RubberbandGradientColor1, this.RubberbandGradientColor2);
												rctGradient = rctPositiveSelectionClientInside;
											}
											else
											{
												lgb = new LinearGradientBrush(ptMouseDown, new Point(e.X, e.Y), this.RubberbandGradientColor1, this.RubberbandGradientColor2);
												rctGradient = rctPositiveSelectionClient;
											}
										}
										else if(this.RubberbandGradientPaintMode == GradientPaintMode.Angle)
										{
											if(this.UseRubberbandBorder)
											{
												lgb = new LinearGradientBrush(rctPositiveSelectionClientInside, this.RubberbandGradientColor1, this.RubberbandGradientColor2, this.RubberbandGradientAngle, true);
												rctGradient = rctPositiveSelectionClientInside;
											}
											else
											{
												lgb = new LinearGradientBrush(rctPositiveSelectionClient, this.RubberbandGradientColor1, this.RubberbandGradientColor2, this.RubberbandGradientAngle, true);
												rctGradient = rctPositiveSelectionClient;
											}
										}
										else if(this.RubberbandGradientPaintMode == GradientPaintMode.LinearGradientMode)
										{
											if(this.UseRubberbandBorder)
											{
												lgb = new LinearGradientBrush(rctPositiveSelectionClientInside, this.RubberbandGradientColor1, this.RubberbandGradientColor2, this.RubberbandGradientLinearGradientMode);
												rctGradient = rctPositiveSelectionClientInside;
											}
											else
											{
												lgb = new LinearGradientBrush(rctPositiveSelectionClient, this.RubberbandGradientColor1, this.RubberbandGradientColor2, this.RubberbandGradientLinearGradientMode);
												rctGradient = rctPositiveSelectionClient;
											}
										}


										if(this.RubberbandGradientShape == GradientShape.Triangular)
										{
											lgb.SetBlendTriangularShape(this.RubberbandGradientShapeFocus, this.RubberbandGradientShapeScale);
										}
										else if(this.RubberbandGradientShape == GradientShape.SigmaBell)
										{
											lgb.SetSigmaBellShape(this.RubberbandGradientShapeFocus, this.RubberbandGradientShapeScale);
										}
										else if(this.RubberbandGradientShape == GradientShape.Blend && this.RubberbandGradientBlend.Length > 0)
										{
											Blend bl = new Blend(this.RubberbandGradientBlend.Length);

											bl.Factors = MWRubberbandGradientBlend.GetFactorArray(this.RubberbandGradientBlend);
											bl.Positions = MWRubberbandGradientBlend.GetPositionArray(this.RubberbandGradientBlend);

											lgb.Blend = bl;
										}
										else if(this.RubberbandGradientShape == GradientShape.ColorBlend && this.RubberbandGradientColorBlend.Length > 0)
										{
											ColorBlend cbl = new ColorBlend(this.RubberbandGradientColorBlend.Length);

											cbl.Colors = MWRubberbandGradientColorBlend.GetColorArray(this.RubberbandGradientColorBlend);
											cbl.Positions = MWRubberbandGradientColorBlend.GetPositionArray(this.RubberbandGradientColorBlend);

											lgb.InterpolationColors = cbl;
										}


										this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
										this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
										this.Invalidate(rctPositiveSelectionClient, false);
										this.Update(false);


										try
										{
											if(this.UseRubberbandBorder)
											{
												g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
											}
										}
										catch
										{
										}

										g.FillRectangle(lgb, rctGradient);

										lgb.Dispose();
									}
									catch
									{
									}

									break;

									#endregion Gradient



									#region PathGradient

									//case RubberbandSelectionMode.PathGradient:
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									//	this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									//	this.Invalidate(rctPositiveSelectionClient, false);
									//	this.Update(false);
									//	//NOT IMPLEMENTED YET.
									//
									//	try
									//	{
									//		if(this.UseRubberbandBorder)
									//		{
									//			g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
									//		}
									//	}
									//	catch
									//	{
									//	}
									//
									//	//DRAW USING PATH GRADIENT BRUSH.
									//
									//	break;

									#endregion PathGradient



									#region Custom

								case RubberbandSelectionMode.Custom:

									//Clearing.
									Rectangle rctClearArea = Rectangle.Union(GetPositiveRectangle(rctInvalidate1), GetPositiveRectangle(rctInvalidate2));
									rctClearArea = Rectangle.Union(rctClearArea, rctPositiveSelectionClient);

									MWRubberbandClearEventArgs ce = new MWRubberbandClearEventArgs(rctClearArea, rctPositiveSelectionClient);

									try
									{
										OnRubberbandClear(ce);

										this.Invalidate(ce.ClearArea, false);
										this.Update();
									}
									catch
									{
										this.Invalidate(rctClearArea, false);
										this.Update();
									}


									//Painting.
									MWRubberbandPaintEventArgs pe = new MWRubberbandPaintEventArgs(g, rctPositiveSelectionClientInside, rctPositiveSelectionClient, ptMouseDownClient, ptMouseMoveClient, this.UseRubberbandBorder);

									try
									{
										OnRubberbandPaint(pe);
									}
									catch
									{
									}

									//Note that the border is painted last since there is the option of overriding the border painting through
									//	the EventHandler.
									if(pe.MWTreeViewPaintBorder)
									{
										try
										{
											g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
										}
										catch
										{
										}
									}

									break;

									#endregion Custom



									#region Image

								case RubberbandSelectionMode.Image:

									Rectangle rctImageSource = Rectangle.Empty;

									ImageAttributes iaImage = new ImageAttributes();

									try
									{
										if(this.RubberbandImageScaleCorrection)
										{
											rctImageSource = new Rectangle(0, 0, this.RubberbandImage.Size.Width - 1, this.RubberbandImage.Size.Height - 1);
										}
										else
										{
											rctImageSource = new Rectangle(Point.Empty, this.RubberbandImage.Size);
										}

										if(this.RubberbandImageTransparency != 1.0f)
										{
											ColorMatrix cmImage = new ColorMatrix();
											//cmImage.Matrix00 = 1.0f;	//Red (already set to 1.0f, so no need to change)
											//cmImage.Matrix11 = 1.0f;	//Green (already set to 1.0f, so no need to change)
											//cmImage.Matrix22 = 1.0f;	//Blue (already set to 1.0f, so no need to change)
											cmImage.Matrix33 = this.RubberbandImageTransparency;	//Transparency
											iaImage.SetColorMatrix(cmImage, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
										}
									}
									catch
									{
									}


									//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
									//ImageAttributes ia = new ImageAttributes();
									//ia.SetGamma(fGamma);
									//ia.SetColorMatrix(cm);
									//g.InterpolationMode = InterpolationMode.Default;
									//g.SmoothingMode = SmoothingMode.Default;
									//g.Transform = new Matrix();


									this.Invalidate(GetPositiveRectangle(rctInvalidate1), false);
									this.Invalidate(GetPositiveRectangle(rctInvalidate2), false);
									this.Invalidate(rctPositiveSelectionClient, false);
									this.Update(false);

									try
									{
										if(this.UseRubberbandBorder)
										{
											g.DrawRectangle(new Pen(this.RubberbandBorderColor, (int)this.RubberbandBorderThickness), rctPositiveSelectionClientFrame);
										}
									}
									catch
									{
									}


									try
									{
										if(this.UseRubberbandBorder)
										{
											if(this.RubberbandImageTransparency != 1.0f)
											{
												g.DrawImage(this.RubberbandImage, rctPositiveSelectionClientInside, 0, 0, rctImageSource.Width, rctImageSource.Height, GraphicsUnit.Pixel, iaImage);
											}
											else
											{
												g.DrawImage(this.RubberbandImage, rctPositiveSelectionClientInside, 0, 0, rctImageSource.Width, rctImageSource.Height, GraphicsUnit.Pixel);
											}
										}
										else
										{
											if(this.RubberbandImageTransparency != 1.0f)
											{
												g.DrawImage(this.RubberbandImage, rctPositiveSelectionClient, 0, 0, rctImageSource.Width, rctImageSource.Height, GraphicsUnit.Pixel, iaImage);
											}
											else
											{
												g.DrawImage(this.RubberbandImage, rctPositiveSelectionClient, 0, 0, rctImageSource.Width, rctImageSource.Height, GraphicsUnit.Pixel);
											}
										}
									}
									catch
									{
									}

									break;

									#endregion Image

								default:
									this.Update(false);
									//Should never end up here.
									break;
							}

							//Testing.
							//g.DrawString(rctPositiveSelectionClient.ToString(), new Font("Arial", 12), new SolidBrush(Color.Black), rctPositiveSelectionClient);

							#endregion RubberbandSelection Painting

							bUpdated = false;

						}
						else if(!(ptMouseDownScreen == this.PointToScreen(ptMouseDown) &&
							ptMouseMoveScreen == this.PointToScreen(new Point(e.X, e.Y))) ||
							bRubberbandStartMoved)
						{
							//End up here if wheel scrolled away and then back to the same position.
							//This will make it update the rubberband to the current position.

							bRubberbandStartMoved = false;
							bUpdated = true;
						}
						else if(this.GetNodeAt(ptMouseDownOrig) != null &&
							this.TreeNodeMouseDownInt != null &&
							this.GetNodeAt(e.X, e.Y) != null &&
							this.GetNodeAt(ptMouseDownOrig) != this.TreeNodeMouseDownInt)
						{
							//End up here if wheel scrolled away.
							//This will make it update the rubberband to the current position.

							bRubberbandStartMoved = false;
							bUpdated = true;
						}


						bRubberbandHasBeenPainted = true;
					}


						#region Selection

					else if(e.Button == MouseButtons.Left && this.FullRowSelect && tn != null)
					{
						if(!bExpandInProgress && !bCollapseInProgress)
						{
							if(!MouseModKeyControl)
							{
								this.ClearSelNodes(this.TreeNodeMouseDownInt);
							}

							if(	this.ConstrainedRubberbandSelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
							{
								if(iMouseDownTreeNodeLevel == GetTreeNodeLevel(this.TreeNodeMouseDownInt))
								{
									SelectNode(this.TreeNodeMouseDownInt, false);
								}
							}
							else if(this.ConstrainedRubberbandSelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
							{
								if(tnMouseDownGrandParent == GetTreeNodeGrandParent(this.TreeNodeMouseDownInt))
								{
									SelectNode(this.TreeNodeMouseDownInt, false);
								}
							}
							else if(this.ConstrainedRubberbandSelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
							{
								if(	iMouseDownTreeNodeLevel == GetTreeNodeLevel(this.TreeNodeMouseDownInt) &&
									tnMouseDownGrandParent == GetTreeNodeGrandParent(this.TreeNodeMouseDownInt))
								{
									SelectNode(this.TreeNodeMouseDownInt, false);
								}
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
								this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
							{
								SelectNode(this.TreeNodeMouseDownInt, false);
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
							{
								if(MouseModKeyAlt)
								{
									SelectNode(this.TreeNodeMouseDownInt, false);
								}
								else
								{
									SelectBranch(this.TreeNodeMouseDownInt, true, true, false);
								}
							}
							else
							{
								if(MouseModKeyAlt)
								{
									SelectBranch(this.TreeNodeMouseDownInt, true, true, false);
								}
								else
								{
									SelectNode(this.TreeNodeMouseDownInt, false);
								}
							}
						}
					}

						#endregion Selection


					else if(bRubberbandHasBeenPainted)
					{
						switch(this.RubberbandSelection)
						{
							case RubberbandSelectionMode.RubberbandDashed:
								ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Dashed);
								break;

							case RubberbandSelectionMode.RubberbandSolid:
								ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Thick);
								break;

							case RubberbandSelectionMode.SelectionFrame:
							case RubberbandSelectionMode.Frame:
							case RubberbandSelectionMode.Fill:
							case RubberbandSelectionMode.Hatch:
								//case RubberbandSelectionMode.Texture:
							case RubberbandSelectionMode.Gradient:
								//case RubberbandSelectionMode.PathGradient:
							case RubberbandSelectionMode.Image:
								this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
								this.Update(false);
								break;

							case RubberbandSelectionMode.Custom:
								MWRubberbandClearEventArgs ce = new MWRubberbandClearEventArgs(GetPositiveRectangle(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y)), rctRubberbandArea);

								try
								{
									OnRubberbandClear(ce);

									this.Invalidate(ce.ClearArea, false);
									this.Update();
								}
								catch
								{
									this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
									this.Update();
								}

								break;

							default:
								//RubberbandSelectionMode.None ends up here
								break;
						}

						bRubberbandHasBeenPainted = false;
					}
				}
				else if(bRubberbandHasBeenPainted)
				{
					switch(this.RubberbandSelection)
					{
						case RubberbandSelectionMode.RubberbandDashed:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Dashed);
							break;

						case RubberbandSelectionMode.RubberbandSolid:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Thick);
							break;

						case RubberbandSelectionMode.SelectionFrame:
						case RubberbandSelectionMode.Frame:
						case RubberbandSelectionMode.Fill:
						case RubberbandSelectionMode.Hatch:
							//case RubberbandSelectionMode.Texture:
						case RubberbandSelectionMode.Gradient:
							//case RubberbandSelectionMode.PathGradient:
						case RubberbandSelectionMode.Image:
							this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
							this.Update(false);
							break;

						case RubberbandSelectionMode.Custom:
							MWRubberbandClearEventArgs ce = new MWRubberbandClearEventArgs(GetPositiveRectangle(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y)), rctRubberbandArea);

							try
							{
								OnRubberbandClear(ce);

								this.Invalidate(ce.ClearArea, false);
								this.Update();
							}
							catch
							{
								this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
								this.Update();
							}

							break;

						default:
							//RubberbandSelectionMode.None ends up here
							break;
					}

					bRubberbandHasBeenPainted = false;
				}
			}

			base.OnMouseMove(e);
		}

		#endregion OnMouseMove



		#region MoveRubberbandStart

		/// <summary>
		/// If the Control is scrolled (up or down) while the rubberband is being painted the starting point (vertically) of the rubberband has
		///		to stay on the same TreeNode, not the same Y-coordinate.
		///	Try this by dragging a selection so that the rubberband becomes visible and then without releasing the left mouse button scroll up
		///		or down using the (vertical) mouse wheel.
		/// </summary>
		private void MoveRubberbandStart()
		{
			bLabelEditAllowed = false;

			ptMouseDown = ptMouseDownOrig;
			TreeNode tnOldMouseDown = this.GetNodeAt(ptMouseDown);

			bool bPrev = false;
			int iY = 0;

			if(tnOldMouseDown != this.TreeNodeMouseDownInt)
			{
				while(tnOldMouseDown != null && tnOldMouseDown.PrevVisibleNode != null)
				{
					iY += this.ItemHeight;

					if(tnOldMouseDown.PrevVisibleNode == this.TreeNodeMouseDownInt)
					{
						bPrev = true;
						ptMouseDown = new Point(ptMouseDown.X, Math.Max(ptMouseDown.Y - iY, 0));
						break;
					}

					tnOldMouseDown = tnOldMouseDown.PrevVisibleNode;
				}

				if(!bPrev)
				{
					tnOldMouseDown = this.GetNodeAt(ptMouseDown);
					iY = 0;

					while(tnOldMouseDown != null && tnOldMouseDown.NextVisibleNode != null)
					{
						iY += this.ItemHeight;

						if(tnOldMouseDown.NextVisibleNode == this.TreeNodeMouseDownInt)
						{
							ptMouseDown = new Point(ptMouseDown.X, Math.Min(ptMouseDown.Y + iY, this.ClientRectangle.Height));
							break;
						}

						tnOldMouseDown = tnOldMouseDown.NextVisibleNode;
					}
				}
			}

			//To reduce flickering.
			if(ptMouseDown != ptMouseDownOrig)
			{
				bRubberbandStartMoved = true;
			}
			else
			{
				bRubberbandStartMoved = false;
			}
		}

		#endregion MoveRubberbandStart



		#region OnMouseHover

		/// <summary>
		/// Standard OnMouseHover EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnMouseHover(EventArgs e)
		{
			if(this.UseRubberbandUpdateMouseHover && Control.MouseButtons == MouseButtons.Left)
			{
				Point ptMousePositionClient = this.PointToClient(Control.MousePosition);
				OnMouseMove(new MouseEventArgs(Control.MouseButtons, 0, ptMousePositionClient.X, ptMousePositionClient.Y, 0));
			}

			base.OnMouseHover(e);
		}

		#endregion OnMouseHover



		#region OnMouseWheel

		/// <summary>
		/// Standard OnMouseWheel EventHandler.
		/// Note that not all mice with wheel(s) generate this event (e.g. mine doesn't, my keyboard does however).
		/// </summary>
		/// <param name="e">Standard MouseEventArgs object.</param>
		protected override void OnMouseWheel(MouseEventArgs e)
		{
			if(Control.MouseButtons == MouseButtons.Left)
			{
				Point ptMousePositionClient = this.PointToClient(Control.MousePosition);
				OnMouseMove(new MouseEventArgs(Control.MouseButtons, 0, ptMousePositionClient.X, ptMousePositionClient.Y, 0));
			}

			TreeNode tn = this.GetNodeAt(e.X, e.Y);

			if(this.HotTracking)
			{
				if(IsTreeNodeSelected(tn) && IsPointOnTreeNode(tn, new Point(e.X, e.Y)))
				{
					bPaintFocusRectAndHottracking = true;
					this.SelectedNode = tn;
					bPaintFocusRectAndHottracking = false;
				}
				else
				{
					this.SelectedNode = null;
				}
			}

			base.OnMouseWheel(e);
		}

		#endregion OnMouseWheel



		#region OnMouseLeave

		/// <summary>
		/// Standard OnMouseLeave EventHandler.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		protected override void OnMouseLeave(System.EventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(bRubberbandHasBeenPainted)
				{
					switch(this.RubberbandSelection)
					{
						case RubberbandSelectionMode.RubberbandDashed:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Dashed);
							break;

						case RubberbandSelectionMode.RubberbandSolid:
							ControlPaint.DrawReversibleFrame(new Rectangle(ptMouseDownScreen.X, ptMouseDownScreen.Y, ptMouseMoveScreen.X - ptMouseDownScreen.X, ptMouseMoveScreen.Y - ptMouseDownScreen.Y), this.BackColor, FrameStyle.Thick);
							break;

						case RubberbandSelectionMode.SelectionFrame:
						case RubberbandSelectionMode.Frame:
						case RubberbandSelectionMode.Fill:
						case RubberbandSelectionMode.Hatch:
							//case RubberbandSelectionMode.Texture:
						case RubberbandSelectionMode.Gradient:
							//case RubberbandSelectionMode.PathGradient:
						case RubberbandSelectionMode.Image:
							this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
							this.Update();
							break;

						case RubberbandSelectionMode.Custom:
							MWRubberbandClearEventArgs ce = new MWRubberbandClearEventArgs(GetPositiveRectangle(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y)), rctRubberbandArea);

							try
							{
								OnRubberbandClear(ce);

								this.Invalidate(ce.ClearArea, false);
								this.Update();
							}
							catch
							{
								this.Invalidate(new Rectangle(ptMouseDownClient.X, ptMouseDownClient.Y, ptMouseMoveClient.X - ptMouseDownClient.X, ptMouseMoveClient.Y - ptMouseDownClient.Y), false);
								this.Update();
							}

							break;

						default:
							//RubberbandSelectionMode.None ends up here
							break;
					}

					if(!this.AllowOutOfFocusRubberbandSelection)
					{
						tRubberbandUpdate.Stop();
					}

					bRubberbandHasBeenPainted = false;


					//Make sure that the SelNode gets set when the mouse leaves the MWTreeView, if there are any selected TreeNodes that is.
					TreeNode tn = GetNodeAt(ptMouseMoveClient);

					if(this.SelNodes.Count > 0 && tn != null)
					{
						if(IsTreeNodeSelected(tn))
						{
							this.SelNodeInt = tn;
						}
						else if(this.SelNodes.Count > 0)
						{
							tn = this.TopNode;

							//Look for the last visible TreeNode
							while(tn != tn.NextVisibleNode && tn.NextVisibleNode != null)
							{
								tn = tn.NextVisibleNode;
							}

							//Iterate from the bottom and up and set the the SelNode property to the first selected TreeNode encountered.
							while(tn != tn.PrevVisibleNode && tn.PrevVisibleNode != null)
							{
								if(IsTreeNodeSelected(tn))
								{
									break;
								}
								else
								{
									tn = tn.PrevVisibleNode;
								}
							}

							if(tn != null)
							{
								this.SelNodeInt = tn;
							}
						}
					}
				}
			}

			base.OnMouseLeave(e);
		}

		#endregion OnMouseLeave

		#endregion Mouse (OnMouseDown, OnMouseUp, OnMouseMove (& MoveRubberbandStart) & OnMouseLeave)



		#region Selection (OnBeforeSelect, BeforeSelectMethod & OnAfterSelect)

		#region OnBeforeSelect

		/// <summary>
		/// Standard OnBeforeSelect EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewCancelEventArgs object.</param>
		protected override void OnBeforeSelect(System.Windows.Forms.TreeViewCancelEventArgs e)
		{
			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				if(!bPaintFocusRectAndHottracking)
				{
					if(	this.LabelEdit &&
						bLabelEditAllowed &&
						this.SelNode == e.Node &&
						!MouseModKeyControl &&
						!MouseModKeyShift &&
						!MouseModKeyAlt)
					{
						BeginEditMethod(this.SelNode);
					}

					if(e.Action != TreeViewAction.Unknown && e.Action != TreeViewAction.ByKeyboard && !this.FullRowSelect)
					{
						BeforeSelectMethod(e.Node);
					}

					e.Cancel = true;

					if(!IsTreeNodeSelected(e.Node))
					{
						EnsureSelNodeNotNull();
					}
				}
			}

			base.OnBeforeSelect(e);
		}

		#endregion OnBeforeSelect



		#region BeforeSelectMethod

		/// <summary>
		/// Main method used when selecting/deselecting TreeNodes using the mouse.
		/// </summary>
		/// <param name="tnMain">Node to be selected/deselected.</param>
		private void BeforeSelectMethod(TreeNode tnMain)
		{
			int iSelNodeTreeNodeLevel = -1;
			TreeNode tnSelNodeGrandParent = null;

			bool bTnMainSelectedOld = IsTreeNodeSelected(tnMain);

			if(MouseModKeyControl && !MouseModKeyShift)
			{
				if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
				{
					if(!IsTreeNodeSelected(tnMain.Parent))
					{
						if(IsTreeNodeSelected(tnMain))
						{
							DeselectBranch(tnMain, true, true);
						}
						else
						{
							SelectBranch(tnMain, true, true, true);
						}
					}
				}
				else if(MouseModKeyAlt)
				{
					if(IsTreeNodeSelected(tnMain))
					{
						DeselectBranch(tnMain, true, true);
					}
					else
					{
						SelectBranch(tnMain, true, true, true);
					}
				}
				else
				{
					ToggleNode(tnMain, true);
				}
			}
			else
			{
				if(MouseModKeyShift)
				{
					if(!MouseModKeyControl)
					{
						ClearSelNodes();
					}

					TreeNode tn = this.SelNode;

					SelectNode(this.SelNode, false);
					iSelNodeTreeNodeLevel = GetTreeNodeLevel(this.SelNode);
					tnSelNodeGrandParent = GetTreeNodeGrandParent(this.SelNode);

					bool bPrevious = false;
					TreeNode tnTemp = this.SelNode;
					while(tnTemp != null && tnTemp.PrevVisibleNode != null && tnTemp != tnMain)
					{
						if(tnTemp.PrevVisibleNode == tnMain)
						{
							bPrevious = true;
							break;
						}
						tnTemp = tnTemp.PrevVisibleNode;
					}

					if(bPrevious)
					{
						while(tn != null && tn.PrevVisibleNode != null && tn != tnMain)
						{
							if(	this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
							{
								if(iSelNodeTreeNodeLevel == GetTreeNodeLevel(tn.PrevVisibleNode))
								{
									SelectNode(tn.PrevVisibleNode, false);
								}
							}
							else if(this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
							{
								if(tnSelNodeGrandParent == GetTreeNodeGrandParent(tn.PrevVisibleNode))
								{
									SelectNode(tn.PrevVisibleNode, false);
								}
							}
							else if(this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
							{
								if(	iSelNodeTreeNodeLevel == GetTreeNodeLevel(tn.PrevVisibleNode) &&
									tnSelNodeGrandParent == GetTreeNodeGrandParent(tn.PrevVisibleNode))
								{
									SelectNode(tn.PrevVisibleNode, false);
								}
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
								this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
							{
								SelectNode(tn.PrevVisibleNode, false);
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
							{
								SelectBranch(tn.PrevVisibleNode, true, bPrevious, true);
							}
							else
							{
								if(MouseModKeyAlt)
								{
									SelectBranch(tn.PrevVisibleNode, true, bPrevious, true);
								}
								else
								{
									SelectNode(tn.PrevVisibleNode, false);
								}
							}

							tn = tn.PrevVisibleNode;
						}
					}
					else
					{
						while(tn != null && tn.NextVisibleNode != null && tn != tnMain)
						{
							if(	this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
							{
								if(iSelNodeTreeNodeLevel == GetTreeNodeLevel(tn.NextVisibleNode))
								{
									SelectNode(tn.NextVisibleNode, false);
								}
							}
							else if(this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
							{
								if(tnSelNodeGrandParent == GetTreeNodeGrandParent(tn.NextVisibleNode))
								{
									SelectNode(tn.NextVisibleNode, false);
								}
							}
							else if(this.ConstrainedShiftKeySelection &&
								this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
							{
								if(	iSelNodeTreeNodeLevel == GetTreeNodeLevel(tn.NextVisibleNode) &&
									tnSelNodeGrandParent == GetTreeNodeGrandParent(tn.NextVisibleNode))
								{
									SelectNode(tn.NextVisibleNode, false);
								}
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
								this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
								this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
							{
								SelectNode(tn.NextVisibleNode, false);
							}
							else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
							{
								SelectBranch(tn.NextVisibleNode, true, bPrevious, true);
							}
							else
							{
								if(MouseModKeyAlt)
								{
									SelectBranch(tn.NextVisibleNode, true, bPrevious, true);
								}
								else
								{
									SelectNode(tn.NextVisibleNode, false);
								}
							}

							//Moved this statement out of the else-statement above. Thanks to Jeff Hansen for finding the bug.
							tn = tn.NextVisibleNode;
						}
					}

					if(	this.ConstrainedShiftKeySelection &&
						this.MultiSelect == TreeViewMultiSelect.MultiSameLevel)
					{
						if(iSelNodeTreeNodeLevel == GetTreeNodeLevel(tnMain))
						{
							SelectNode(tnMain, true);
						}
					}
					else if(this.ConstrainedShiftKeySelection &&
						this.MultiSelect == TreeViewMultiSelect.MultiSameBranch)
					{
						if(tnSelNodeGrandParent == GetTreeNodeGrandParent(tnMain))
						{
							SelectNode(tnMain, false);
						}
					}
					else if(this.ConstrainedShiftKeySelection &&
						this.MultiSelect == TreeViewMultiSelect.MultiSameBranchAndLevel)
					{
						if(	iSelNodeTreeNodeLevel == GetTreeNodeLevel(tnMain) &&
							tnSelNodeGrandParent == GetTreeNodeGrandParent(tnMain))
						{
							SelectNode(tnMain, true);
						}
					}
					else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
					{
						SelectBranch(tnMain, true, true, true);
					}
					else
					{
						SelectNode(tnMain, true);
					}
				}
				else
				{
					ClearSelNodes();

					if(	MouseModKeyAlt &&
						this.MultiSelect != TreeViewMultiSelect.MultiSameLevel &&
						this.MultiSelect != TreeViewMultiSelect.MultiSameBranchAndLevel &&
						this.MultiSelect != TreeViewMultiSelect.MultiAllDescendants)
					{
						SelectBranch(tnMain, true, true, true);
					}
					else if(this.MultiSelect == TreeViewMultiSelect.MultiAllDescendants)
					{
						SelectBranch(tnMain, true, true, true);
					}
					else
					{
						SelectNode(tnMain, true);
					}
				}
			}

			bool bTnMainSelectedNew = IsTreeNodeSelected(tnMain);

			if(	this.LabelEdit &&
				bLabelEditAllowed &&
				tnMain != null &&
				bTnMainSelectedOld && bTnMainSelectedNew &&
				!MouseModKeyControl &&
				!MouseModKeyShift &&
				!MouseModKeyAlt
				)
			{
				BeginEditMethod(tnMain);
			}
		}

		#endregion BeforeSelectMethod



		#region OnAfterSelect

		/// <summary>
		/// Standard OnAfterSelect EventHandler.
		/// </summary>
		/// <param name="e">Standard TreeViewCancelEventArgs object.</param>
		protected override void OnAfterSelect(TreeViewEventArgs e)
		{
			base.OnAfterSelect(e);

			if(this.MultiSelect != TreeViewMultiSelect.Classic)
			{
				TreeNode tn = this.GetNodeAt(this.PointToClient(Control.MousePosition));

				if(	this.SelectedNode != null &&
					//bPaintFocusRectAndHottracking &&
					this.HotTracking &&
					IsPointOnTreeNode(tn, this.PointToClient(Control.MousePosition)))
				{
					bPaintFocusRectAndHottracking = true;
					this.SelectedNode = tn;
					bPaintFocusRectAndHottracking = false;
				}
				else if(this.UseExtendedSelectionColors)
				{
					bPaintFocusRectAndHottracking = true;
					this.SelectedNode = null;
					bPaintFocusRectAndHottracking = false;
				}
			}
		}

		#endregion OnAfterSelect

		#endregion Selection (OnBeforeSelect, BeforeSelectMethod & OnAfterSelect)


		#endregion Overridden EventHandlers



		#region Browsable EventHandlers for EventHandlers

		#region Category 'Behavior, MW - Rubberband-Selection Custom' Properties

		#region Rubberband clearing EventHandler (EventHandler used from OnMouseMove)

		/// <summary>
		/// Occurs when the rubberband is cleared.
		/// Note that this should only be used if there is any painting done outside of the borders of the regular rubberband painting area.
		/// Note that even if this is used the ClearArea might have to be increased quite a lot because of the latency of events being fired.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Custom"),
		Description("Occurs when the rubberband is cleared.")
		]
		public event MWControlSuite.MWRubberbandClearEventHandler RubberbandClear;

		/// <summary>
		/// Raises the RubberbandClear event.
		/// </summary>
		/// <param name="e">Standard MWRubberbandPaintEventArgs object.</param>
		public void OnRubberbandClear(MWRubberbandClearEventArgs e)
		{
			if(RubberbandClear != null)
			{
				RubberbandClear(this, e);
			}
		}

		#endregion Rubberband clearing EventHandler (EventHandler used from OnMouseMove)



		#region Rubberband painting EventHandler (EventHandler used from OnMouseMove)

		/// <summary>
		/// Occurs when the rubberband is painted.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Custom"),
		Description("Occurs when the rubberband is painted.")
		]
		public event MWControlSuite.MWRubberbandPaintEventHandler RubberbandPaint;

		/// <summary>
		/// Raises the RubberbandPaint event.
		/// </summary>
		/// <param name="e">Standard MWRubberbandPaintEventArgs object.</param>
		public void OnRubberbandPaint(MWRubberbandPaintEventArgs e)
		{
			if(RubberbandPaint != null)
			{
				RubberbandPaint(this, e);
			}
		}

		#endregion Rubberband painting EventHandler (EventHandler used from OnMouseMove)

		#endregion Category 'Behavior, MW - Rubberband-Selection Custom' Properties

		#endregion Browsable EventHandlers for EventHandlers



		#region Newed Properties

		#region Enabled

		/// <summary>
		/// Standard Enabled property that also handles the enabling and disabling of TreeNodes.
		/// </summary>
		public new bool Enabled
		{
			get
			{
				return base.Enabled;
			}
			set
			{
				if(this.MultiSelect != TreeViewMultiSelect.Classic)
				{
					bool bEnabledOld = base.Enabled;

					base.Enabled = value;

					//base.Enabled is used instead of value just in case it is not set.
					if(bEnabledOld != base.Enabled)
					{
						if(base.Enabled)
						{
							EnableNodes();
						}
						else
						{
							DisableNodes();
						}

						if(!this.Focused && !this.UseExtendedDisabledColors)
						{
							ActivateOrDeactivateSelNodes();
						}
					}
				}
				else
				{
					base.Enabled = value;
				}
			}
		}

		#endregion Enabled



		#region HideSelection

		/// <summary>
		/// Standard HideSelection property that also handles the activation and deactivation of selected TreeNodes.
		/// </summary>
		public new bool HideSelection
		{
			get
			{
				return base.HideSelection;
			}
			set
			{
				if(this.MultiSelect != TreeViewMultiSelect.Classic)
				{
					bool bHideSelectionOld = base.HideSelection;

					base.HideSelection = value;

					//base.HideSelection is used instead of value just in case it is not set.
					if(bHideSelectionOld != base.HideSelection)
					{
						if(this.Enabled)
						{
							ActivateOrDeactivateSelNodes();
						}
						else
						{
							EnableNodes();
							DisableNodes();
						}
					}
				}
				else
				{
					base.HideSelection = value;
				}
			}
		}

		#endregion HideSelection



		#region SelectedNode

		/// <summary>
		/// Standard SelectedNode property that returns the selected TreeNode.
		/// For multiselect (where more than one TreeNode is selected) there is always one TreeNode that is considered to be the most recently
		///		selected one - it is called SelNode. This get-property returns the same TreeNode as SelNode (in fact it returns SelNode).
		///	This override has been done to make the MWTreeView behave a little bit more like the standard TreeView.
		/// </summary>
		public new TreeNode SelectedNode
		{
			get
			{
				if(this.MultiSelect != TreeViewMultiSelect.Classic)
				{
					return this.SelNode;
				}
				else
				{
					return base.SelectedNode;
				}
			}
			set
			{
				base.SelectedNode = value;
			}
		}

		#endregion SelectedNode

		#endregion Newed Properties



		#region Newed Methods

		#region Update

		/// <summary>
		/// Standard Update method that updates the invalidated regions of the MWTreeView.
		/// This Update also sets the bUpdated variable to true which is used in OnMouseMove to know whether to paint the rubberband or not.
		/// </summary>
		public new void Update()
		{
			base.Update();

			bUpdated = true;
		}

		/// <summary>
		/// Update method that if called with an argument of true 'notifies' the OnMouseMove EventHandler or if called with an
		///		argument of false does a regular update of the invalidated regions of the MWTreeView.
		///	Note that the word notify is used very losely here. There is no proper notification going on. OnMouseMove simply
		///		repaints the rubberband if the regular Update method is called or if this overload is called with an argument
		///		of true.
		/// </summary>
		/// <param name="bNotifyOnMouseMove"></param>
		public void Update(bool bNotifyOnMouseMove)
		{
			if(bNotifyOnMouseMove)
			{
				this.Update();
			}
			else
			{
				base.Update();
			}
		}

		#endregion Update

		#endregion Newed Methods

		#endregion Overridden and Newed Properties, EventHandlers and Methods



		#region Delegates

		#region MWCancelEventHandler delegate

		/// <summary>
		/// A delegate for event MWCancelEventHandler.
		/// </summary>
		public delegate void MWCancelEventHandler(object sender, MWControlSuite.MWCancelEventArgs e);

		#endregion MWCancelEventHandler delegate



		#region MWPropertyEventHandler delegate

		/// <summary>
		/// A delegate for event MWPropertyEventHandler.
		/// </summary>
		public delegate void MWPropertyEventHandler(object sender, MWControlSuite.MWPropertyEventArgs e);

		#endregion MWPropertyEventHandler delegate



		#region MWRubberbandPaintEventHandler delegate

		/// <summary>
		/// A delegate for event MWRubberbandPaintEventHandler.
		/// </summary>
		public delegate void MWRubberbandPaintEventHandler(object sender, MWControlSuite.MWRubberbandPaintEventArgs e);

		#endregion MWRubberbandPaintEventHandler delegate



		#region MWRubberbandClearEventHandler delegate

		/// <summary>
		/// A delegate for event MWRubberbandClearEventHandler.
		/// </summary>
		public delegate void MWRubberbandClearEventHandler(object sender, MWControlSuite.MWRubberbandClearEventArgs e);

		#endregion MWRubberbandClearEventHandler delegate

		#endregion Delegates



		#region EventHandlers for Components

		#region EventHandlers for LabelEditing

		#region EventHandlers for Timer tLabelEdit (Tick)

		/// <summary>
		/// Standard Tick EventHandler for Timer tLabelEdit.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void tLabelEdit_Tick(object sender, System.EventArgs e)
		{
			tLabelEdit.Stop();

			StartLabelEdit();
		}

		#endregion EventHandlers for Timer tLabelEdit (Tick)



		#region EventHandlers for Timer tLabelEditAllowed (Tick)

		/// <summary>
		/// Standard Tick EventHandler for Timer tLabelEditAllowed.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void tLabelEditAllowed_Tick(object sender, System.EventArgs e)
		{
			tLabelEditAllowed.Stop();
			bLabelEditAllowed = true;
		}

		#endregion EventHandlers for Timer tLabelEditAllowed (Tick)

		#endregion EventHandlers for LabelEditing



		#region EventHandlers for RubberbandSelectionAllowed (Tick)

		#region EventHandlers for Timer tRubberbandSelectionAllowedExpand (Tick)

		/// <summary>
		/// Standard Tick EventHandler for Timer tRubberbandSelectionAllowedExpand.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void tRubberbandSelectionAllowedExpand_Tick(object sender, System.EventArgs e)
		{
			tRubberbandSelectionAllowedExpand.Stop();
			bExpandInProgress = false;
		}

		#endregion EventHandlers for Timer tRubberbandSelectionAllowedExpand (Tick)



		#region EventHandlers for Timer tRubberbandSelectionAllowedCollapse (Tick)

		/// <summary>
		/// Standard Tick EventHandler for Timer tRubberbandSelectionAllowedCollapse.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void tRubberbandSelectionAllowedCollapse_Tick(object sender, System.EventArgs e)
		{
			tRubberbandSelectionAllowedCollapse.Stop();
			bCollapseInProgress = false;
		}

		#endregion EventHandlers for Timer tRubberbandSelectionAllowedCollapse (Tick)

		#endregion EventHandlers for RubberbandSelectionAllowed



		#region EventHandlers for rubberband updates

		#region EventHandlers for Timer tRubberbandUpdate (Tick)

		/// <summary>
		/// Standard Tick EventHandler for Timer tRubberbandUpdate.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard EventArgs object.</param>
		private void tRubberbandUpdate_Tick(object sender, System.EventArgs e)
		{
			if(this.UseRubberbandUpdateTimer)
			{
				Point ptMousePositionClient = this.PointToClient(Control.MousePosition);

				if(bActive && Control.MouseButtons == MouseButtons.Left && this.ClientRectangle.Contains(ptMousePositionClient))
				{
					OnMouseMove(new MouseEventArgs(Control.MouseButtons, 0, ptMousePositionClient.X, ptMousePositionClient.Y, 0));
				}
				else if(!bActive && !this.AllowOutOfFocusRubberbandSelection)
				{
					tRubberbandUpdate.Stop();
				}
			}
			else
			{
				tRubberbandUpdate.Stop();
			}
		}

		#endregion EventHandlers for Timer tRubberbandUpdate (Tick)

		#endregion EventHandlers for rubberband updates

		#endregion EventHandlers for Components



		#region Properties with EventHandlers

		#region Browsable Properties

		#region Category 'Appearance, MW - Disable/Enable' Properties

		#region DisabledTreeNodeBackColor Property and EventHandlers

		#region DisabledTreeNodeBackColor Property

		/// <summary>
		/// The BackColor of a Disabled TreeNode.
		/// Note that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("The BackColor of a Disabled TreeNode.\nNote that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true."),
		DefaultValue(typeof(Color), "Control")
		]
		public Color DisabledTreeNodeBackColor
		{
			get
			{
				return cDisabledTreeNodeBackColor;
			}
			set
			{
				if(cDisabledTreeNodeBackColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cDisabledTreeNodeBackColor, value);
					OnBeforeDisabledTreeNodeBackColorChanged(e);

					if(!e.Cancel)
					{
						cDisabledTreeNodeBackColor = value;

						if(!this.Enabled)
						{
							EnableNodes();
							DisableNodes();
						}

						OnAfterDisabledTreeNodeBackColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion DisabledTreeNodeBackColor Property



		#region DisabledTreeNodeBackColor EventHandlers

		/// <summary>
		/// Occurs before the DisabledTreeNodeBackColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs before the DisabledTreeNodeBackColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeDisabledTreeNodeBackColorChanged;

		/// <summary>
		/// Raises the BeforeDisabledTreeNodeBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeDisabledTreeNodeBackColorChanged(MWCancelEventArgs e)
		{
			if(BeforeDisabledTreeNodeBackColorChanged != null)
			{
				BeforeDisabledTreeNodeBackColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the DisabledTreeNodeBackColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs after the DisabledTreeNodeBackColor property has changed.")
		]
		public event EventHandler AfterDisabledTreeNodeBackColorChanged;

		/// <summary>
		/// Raises the AfterDisabledTreeNodeBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterDisabledTreeNodeBackColorChanged(EventArgs e)
		{
			if(AfterDisabledTreeNodeBackColorChanged != null)
			{
				AfterDisabledTreeNodeBackColorChanged(this, e);
			}
		}

		#endregion DisabledTreeNodeBackColor EventHandlers

		#endregion DisabledTreeNodeBackColor Property and EventHandlers



		#region DisabledTreeNodeForeColor Property and EventHandlers

		#region DisabledTreeNodeForeColor Property

		/// <summary>
		/// The ForeColor of a Disabled TreeNode.
		/// Note that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("The ForeColor of a Disabled TreeNode.\nNote that this property is used when a TreeView's Enabled property is set to false and the UseExtendedDisabledColors property is set to true."),
		DefaultValue(typeof(Color), "ControlDark")
		]
		public Color DisabledTreeNodeForeColor
		{
			get
			{
				return cDisabledTreeNodeForeColor;
			}
			set
			{
				if(cDisabledTreeNodeForeColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cDisabledTreeNodeForeColor, value);
					OnBeforeDisabledTreeNodeForeColorChanged(e);

					if(!e.Cancel)
					{
						cDisabledTreeNodeForeColor = value;

						if(!this.Enabled)
						{
							EnableNodes();
							DisableNodes();
						}

						OnAfterDisabledTreeNodeForeColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion DisabledTreeNodeForeColor Property



		#region DisabledTreeNodeForeColor EventHandlers

		/// <summary>
		/// Occurs before the DisabledTreeNodeForeColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs before the DisabledTreeNodeForeColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeDisabledTreeNodeForeColorChanged;

		/// <summary>
		/// Raises the BeforeDisabledTreeNodeForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeDisabledTreeNodeForeColorChanged(MWCancelEventArgs e)
		{
			if(BeforeDisabledTreeNodeForeColorChanged != null)
			{
				BeforeDisabledTreeNodeForeColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the DisabledTreeNodeForeColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs after the DisabledTreeNodeForeColor property has changed.")
		]
		public event EventHandler AfterDisabledTreeNodeForeColorChanged;

		/// <summary>
		/// Raises the AfterDisabledTreeNodeForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterDisabledTreeNodeForeColorChanged(EventArgs e)
		{
			if(AfterDisabledTreeNodeForeColorChanged != null)
			{
				AfterDisabledTreeNodeForeColorChanged(this, e);
			}
		}

		#endregion DisabledTreeNodeForeColor EventHandlers

		#endregion DisabledTreeNodeForeColor Property and EventHandlers



		#region UseExtendedDisabledColors Property and EventHandlers

		#region UseExtendedDisabledColors Property

		/// <summary>
		/// True if TreeNodes with other than plain colors (BackColor = SystemColors.Window and ForeColor = SystemColors.WindowText) should look properly disabled or false otherwise (standard MS TreeView appearance).
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("True if TreeNodes with other than plain colors (BackColor = SystemColors.Window and ForeColor = SystemColors.WindowText) should look properly disabled or false otherwise (standard MS TreeView appearance)."),
		DefaultValue(true)
		]
		public bool UseExtendedDisabledColors
		{
			get
			{
				return bUseExtendedDisabledColors;
			}
			set
			{
				if(bUseExtendedDisabledColors != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bUseExtendedDisabledColors, value);
					OnBeforeUseExtendedDisabledColorsChanged(e);

					if(!e.Cancel)
					{
						bUseExtendedDisabledColors = value;

						if(!this.Enabled)
						{
							EnableNodes();
							DisableNodes();
						}

						OnAfterUseExtendedDisabledColorsChanged(new EventArgs());
					}
				}
			}
		}

		#endregion UseExtendedDisabledColors Property



		#region UseExtendedDisabledColors EventHandlers

		/// <summary>
		/// Occurs before the UseExtendedDisabledColors property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs before the UseExtendedDisabledColors property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeUseExtendedDisabledColorsChanged;

		/// <summary>
		/// Raises the BeforeUseExtendedDisabledColorsChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeUseExtendedDisabledColorsChanged(MWCancelEventArgs e)
		{
			if(BeforeUseExtendedDisabledColorsChanged != null)
			{
				BeforeUseExtendedDisabledColorsChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the UseExtendedDisabledColors property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Disable/Enable"),
		Description("Occurs after the UseExtendedDisabledColors property has changed.")
		]
		public event EventHandler AfterUseExtendedDisabledColorsChanged;

		/// <summary>
		/// Raises the AfterUseExtendedDisabledColorsChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterUseExtendedDisabledColorsChanged(EventArgs e)
		{
			if(AfterUseExtendedDisabledColorsChanged != null)
			{
				AfterUseExtendedDisabledColorsChanged(this, e);
			}
		}

		#endregion UseExtendedDisabledColors EventHandlers

		#endregion UseExtendedDisabledColors Property and EventHandlers

		#endregion Category 'Appearance, MW - Disable/Enable' Properties



		#region Category 'Behavior, MW - Checking' Properties

		#region AllowMultiCheck Property and EventHandlers

		#region AllowMultiCheck Property

		/// <summary>
		/// True if multiple TreeNodes can be checked at once or false otherwise (true is standard for MS TreeView).
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("True if multiple TreeNodes can be checked at once or false otherwise (true is standard for MS TreeView)."),
		DefaultValue(true)
		]
		public bool AllowMultiCheck
		{
			get
			{
				return bAllowMultiCheck;
			}
			set
			{
				if(bAllowMultiCheck != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowMultiCheck, value);
					OnBeforeAllowMultiCheckChanged(e);

					if(!e.Cancel)
					{
						if(!value)
						{
							bAllowMultiCheckChanged = true;

							TreeNode tn = null;

							if(this.CheckedNodes != null)
							{
								foreach(TreeNode tn2 in this.CheckedNodes.Values)
								{
									tn = tn2;
									break;
								}
							}

							this.ClearCheckedNodes();

							if(tn != null)
							{
								CheckNode(tn, true);
							}

							bAllowMultiCheckChanged = false;
						}

						bAllowMultiCheck = value;

						OnAfterAllowMultiCheckChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowMultiCheck Property



		#region AllowMultiCheck EventHandlers

		/// <summary>
		/// Occurs before the AllowMultiCheck property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs before the AllowMultiCheck property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowMultiCheckChanged;

		/// <summary>
		/// Raises the BeforeAllowMultiCheckChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowMultiCheckChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowMultiCheckChanged != null)
			{
				BeforeAllowMultiCheckChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowMultiCheck property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs after the AllowMultiCheck property has changed.")
		]
		public event EventHandler AfterAllowMultiCheckChanged;

		/// <summary>
		/// Raises the AfterAllowMultiCheckChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowMultiCheckChanged(EventArgs e)
		{
			if(AfterAllowMultiCheckChanged != null)
			{
				AfterAllowMultiCheckChanged(this, e);
			}
		}

		#endregion AllowMultiCheck EventHandlers

		#endregion AllowMultiCheck Property and EventHandlers



		#region CheckNodeRegEx Property and EventHandlers

		#region CheckNodeRegEx Property

		/// <summary>
		/// Regular expression that has to be satisfied before a TreeNode can be checked.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Regular expression that has to be satisfied before a TreeNode can be checked."),
		DefaultValue("")
		]
		public string CheckNodeRegEx
		{
			get
			{
				return strCheckNodeRegEx;
			}
			set
			{
				if(strCheckNodeRegEx != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(strCheckNodeRegEx, value);
					OnBeforeCheckNodeRegExChanged(e);

					if(!e.Cancel)
					{
						strCheckNodeRegEx = value;

						EnsureAllCheckedNodesAreAllowed();

						OnAfterCheckNodeRegExChanged(new EventArgs());
					}
				}
			}
		}

		#endregion CheckNodeRegEx Property



		#region CheckNodeRegEx EventHandlers

		/// <summary>
		/// Occurs before the CheckNodeRegEx property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs before the CheckNodeRegEx property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCheckNodeRegExChanged;

		/// <summary>
		/// Raises the BeforeCheckNodeRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCheckNodeRegExChanged(MWCancelEventArgs e)
		{
			if(BeforeCheckNodeRegExChanged != null)
			{
				BeforeCheckNodeRegExChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the CheckNodeRegEx property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs after the CheckNodeRegEx property has changed.")
		]
		public event EventHandler AfterCheckNodeRegExChanged;

		/// <summary>
		/// Raises the AfterCheckNodeRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCheckNodeRegExChanged(EventArgs e)
		{
			if(AfterCheckNodeRegExChanged != null)
			{
				AfterCheckNodeRegExChanged(this, e);
			}
		}

		#endregion CheckNodeRegEx EventHandlers

		#endregion CheckNodeRegEx Property and EventHandlers

		#endregion Category 'Behavior, MW - Checking' Properties



		#region Category 'Behavior, MW - Expanding/Collapsing' Properties

		#region AllowMultipleExpandedPaths Property and EventHandlers

		#region AllowMultipleExpandedPaths Property

		/// <summary>
		/// True if more than one path can be expanded at a time (standard TreeView behavior) or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if more than one path can be expanded at a time (standard TreeView behavior) or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowMultipleExpandedPaths
		{
			get
			{
				return bAllowMultipleExpandedPaths;
			}
			set
			{
				if(bAllowMultipleExpandedPaths != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowMultipleExpandedPaths, value);
					OnBeforeAllowMultipleExpandedPathsChanged(e);

					if(!e.Cancel)
					{
						if(!value)
						{
							if(this.SelNode != null)
							{
								if(this.ExpandBeforeCollapse)
								{
									if(this.SelNode != null)
									{
										this.SelNode.EnsureVisible();
									}
								}

								TreeNode tnSelNodeGrandParent = GetTreeNodeGrandParent(this.SelNode);

								this.SelNode.EnsureVisible();

								if(tnSelNodeGrandParent != null)
								{
									foreach(TreeNode tn in this.Nodes)
									{
										if(tn != tnSelNodeGrandParent)
										{
											tn.Collapse();
										}
									}
								}

								if(!this.ExpandBeforeCollapse)
								{
									if(this.SelNode != null)
									{
										this.SelNode.EnsureVisible();
									}
								}
							}
							else
							{
								foreach(TreeNode tn in this.Nodes)
								{
									tn.Collapse();
								}
							}
						}

						bAllowMultipleExpandedPaths = value;

						OnAfterAllowMultipleExpandedPathsChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowMultipleExpandedPaths Property



		#region AllowMultipleExpandedPaths EventHandlers

		/// <summary>
		/// Occurs before the AllowMultipleExpandedPaths property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the AllowMultipleExpandedPaths property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowMultipleExpandedPathsChanged;

		/// <summary>
		/// Raises the BeforeAllowMultipleExpandedPathsChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowMultipleExpandedPathsChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowMultipleExpandedPathsChanged != null)
			{
				BeforeAllowMultipleExpandedPathsChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowMultipleExpandedPaths property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the AllowMultipleExpandedPaths property has changed.")
		]
		public event EventHandler AfterAllowMultipleExpandedPathsChanged;

		/// <summary>
		/// Raises the AfterAllowMultipleExpandedPathsChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowMultipleExpandedPathsChanged(EventArgs e)
		{
			if(AfterAllowMultipleExpandedPathsChanged != null)
			{
				AfterAllowMultipleExpandedPathsChanged(this, e);
			}
		}

		#endregion AllowMultipleExpandedPaths EventHandlers

		#endregion AllowMultipleExpandedPaths Property and EventHandlers



		#region ExpandBeforeCollapse Property and EventHandlers

		#region ExpandBeforeCollapse Property

		/// <summary>
		/// True if TreeNode expands before other TreeNodes collapse or false if other TreeNodes collapse before TreeNode expands.
		/// Note that this is only used if the AllowMultipleExpandedPaths property is set to false.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if TreeNode expands before other TreeNodes collapse or false if other TreeNodes collapse before TreeNode expands.\nNote that this is only used if the AllowMultipleExpandedPaths property is set to false."),
		DefaultValue(true)
		]
		public bool ExpandBeforeCollapse
		{
			get
			{
				return bExpandBeforeCollapse;
			}
			set
			{
				if(bExpandBeforeCollapse != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bExpandBeforeCollapse, value);
					OnBeforeExpandBeforeCollapseChanged(e);

					if(!e.Cancel)
					{
						bExpandBeforeCollapse = value;

						OnAfterExpandBeforeCollapseChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ExpandBeforeCollapse Property



		#region ExpandBeforeCollapse EventHandlers

		/// <summary>
		/// Occurs before the ExpandBeforeCollapse property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the ExpandBeforeCollapse property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeExpandBeforeCollapseChanged;

		/// <summary>
		/// Raises the BeforeExpandBeforeCollapseChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeExpandBeforeCollapseChanged(MWCancelEventArgs e)
		{
			if(BeforeExpandBeforeCollapseChanged != null)
			{
				BeforeExpandBeforeCollapseChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ExpandBeforeCollapse property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the ExpandBeforeCollapse property has changed.")
		]
		public event EventHandler AfterExpandBeforeCollapseChanged;

		/// <summary>
		/// Raises the AfterExpandBeforeCollapseChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterExpandBeforeCollapseChanged(EventArgs e)
		{
			if(AfterExpandBeforeCollapseChanged != null)
			{
				AfterExpandBeforeCollapseChanged(this, e);
			}
		}

		#endregion ExpandBeforeCollapse EventHandlers

		#endregion ExpandBeforeCollapse Property and EventHandlers



		#region CollapseTreeNodeOnDeselect Property and EventHandlers

		#region CollapseTreeNodeOnDeselect Property

		/// <summary>
		/// True if a TreeNode is collapsed when it is deselected or false otherwise.
		/// Note that this only happens if the TreeNode is expanded.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if a TreeNode is collapsed when it is deselected or false otherwise.\nNote that this only happens if the TreeNode is expanded."),
		DefaultValue(false)
		]
		public bool CollapseTreeNodeOnDeselect
		{
			get
			{
				return bCollapseTreeNodeOnDeselect;
			}
			set
			{
				if(bCollapseTreeNodeOnDeselect != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bCollapseTreeNodeOnDeselect, value);
					OnBeforeCollapseTreeNodeOnDeselectChanged(e);

					if(!e.Cancel)
					{
						bCollapseTreeNodeOnDeselect = value;

						OnAfterCollapseTreeNodeOnDeselectChanged(new EventArgs());
					}
				}
			}
		}

		#endregion CollapseTreeNodeOnDeselect Property



		#region CollapseTreeNodeOnDeselect EventHandlers

		/// <summary>
		/// Occurs before the CollapseTreeNodeOnDeselect property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the CollapseTreeNodeOnDeselect property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCollapseTreeNodeOnDeselectChanged;

		/// <summary>
		/// Raises the BeforeCollapseTreeNodeOnDeselectChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCollapseTreeNodeOnDeselectChanged(MWCancelEventArgs e)
		{
			if(BeforeCollapseTreeNodeOnDeselectChanged != null)
			{
				BeforeCollapseTreeNodeOnDeselectChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the CollapseTreeNodeOnDeselect property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the CollapseTreeNodeOnDeselect property has changed.")
		]
		public event EventHandler AfterCollapseTreeNodeOnDeselectChanged;

		/// <summary>
		/// Raises the AfterCollapseTreeNodeOnDeselectChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCollapseTreeNodeOnDeselectChanged(EventArgs e)
		{
			if(AfterCollapseTreeNodeOnDeselectChanged != null)
			{
				AfterCollapseTreeNodeOnDeselectChanged(this, e);
			}
		}

		#endregion CollapseTreeNodeOnDeselect EventHandlers

		#endregion CollapseTreeNodeOnDeselect Property and EventHandlers



		#region CollapseTreeNodeOnSelect Property and EventHandlers

		#region CollapseTreeNodeOnSelect Property

		/// <summary>
		/// True if a TreeNode is collapsed when it is selected or false otherwise.
		/// Note that this only happens if the TreeNode is expanded.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if a TreeNode is collapsed when it is selected or false otherwise.\nNote that this only happens if the TreeNode is expanded."),
		DefaultValue(false)
		]
		public bool CollapseTreeNodeOnSelect
		{
			get
			{
				return bCollapseTreeNodeOnSelect;
			}
			set
			{
				if(bCollapseTreeNodeOnSelect != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bCollapseTreeNodeOnSelect, value);
					OnBeforeCollapseTreeNodeOnSelectChanged(e);

					if(!e.Cancel)
					{
						bCollapseTreeNodeOnSelect = value;

						OnAfterCollapseTreeNodeOnSelectChanged(new EventArgs());
					}
				}
			}
		}

		#endregion CollapseTreeNodeOnSelect Property



		#region CollapseTreeNodeOnSelect EventHandlers

		/// <summary>
		/// Occurs before the CollapseTreeNodeOnSelect property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the CollapseTreeNodeOnSelect property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCollapseTreeNodeOnSelectChanged;

		/// <summary>
		/// Raises the BeforeCollapseTreeNodeOnSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCollapseTreeNodeOnSelectChanged(MWCancelEventArgs e)
		{
			if(BeforeCollapseTreeNodeOnSelectChanged != null)
			{
				BeforeCollapseTreeNodeOnSelectChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the CollapseTreeNodeOnSelect property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the CollapseTreeNodeOnSelect property has changed.")
		]
		public event EventHandler AfterCollapseTreeNodeOnSelectChanged;

		/// <summary>
		/// Raises the AfterCollapseTreeNodeOnSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCollapseTreeNodeOnSelectChanged(EventArgs e)
		{
			if(AfterCollapseTreeNodeOnSelectChanged != null)
			{
				AfterCollapseTreeNodeOnSelectChanged(this, e);
			}
		}

		#endregion CollapseTreeNodeOnSelect EventHandlers

		#endregion CollapseTreeNodeOnSelect Property and EventHandlers



		#region ExpandTreeNodeOnDeselect Property and EventHandlers

		#region ExpandTreeNodeOnDeselect Property

		/// <summary>
		/// True if a TreeNode is expanded when it is deselected or false otherwise.
		/// Note that this only happens if the TreeNode is collapsed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if a TreeNode is expanded when it is deselected or false otherwise.\nNote that this only happens if the TreeNode is collapsed."),
		DefaultValue(false)
		]
		public bool ExpandTreeNodeOnDeselect
		{
			get
			{
				return bExpandTreeNodeOnDeselect;
			}
			set
			{
				if(bExpandTreeNodeOnDeselect != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bExpandTreeNodeOnDeselect, value);
					OnBeforeExpandTreeNodeOnDeselectChanged(e);

					if(!e.Cancel)
					{
						bExpandTreeNodeOnDeselect = value;

						OnAfterExpandTreeNodeOnDeselectChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ExpandTreeNodeOnDeselect Property



		#region ExpandTreeNodeOnDeselect EventHandlers

		/// <summary>
		/// Occurs before the ExpandTreeNodeOnDeselect property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the ExpandTreeNodeOnDeselect property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeExpandTreeNodeOnDeselectChanged;

		/// <summary>
		/// Raises the BeforeExpandTreeNodeOnDeselectChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeExpandTreeNodeOnDeselectChanged(MWCancelEventArgs e)
		{
			if(BeforeExpandTreeNodeOnDeselectChanged != null)
			{
				BeforeExpandTreeNodeOnDeselectChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ExpandTreeNodeOnDeselect property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the ExpandTreeNodeOnDeselect property has changed.")
		]
		public event EventHandler AfterExpandTreeNodeOnDeselectChanged;

		/// <summary>
		/// Raises the AfterExpandTreeNodeOnDeselectChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterExpandTreeNodeOnDeselectChanged(EventArgs e)
		{
			if(AfterExpandTreeNodeOnDeselectChanged != null)
			{
				AfterExpandTreeNodeOnDeselectChanged(this, e);
			}
		}

		#endregion ExpandTreeNodeOnDeselect EventHandlers

		#endregion ExpandTreeNodeOnDeselect Property and EventHandlers



		#region ExpandTreeNodeOnSelect Property and EventHandlers

		#region ExpandTreeNodeOnSelect Property

		/// <summary>
		/// True if a TreeNode is expanded when it is selected or false otherwise.
		/// Note that this only happens if the TreeNode is collapsed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("True if a TreeNode is expanded when it is selected or false otherwise.\nNote that this only happens if the TreeNode is collapsed."),
		DefaultValue(false)
		]
		public bool ExpandTreeNodeOnSelect
		{
			get
			{
				return bExpandTreeNodeOnSelect;
			}
			set
			{
				if(bExpandTreeNodeOnSelect != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bExpandTreeNodeOnSelect, value);
					OnBeforeExpandTreeNodeOnSelectChanged(e);

					if(!e.Cancel)
					{
						bExpandTreeNodeOnSelect = value;

						OnAfterExpandTreeNodeOnSelectChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ExpandTreeNodeOnSelect Property



		#region ExpandTreeNodeOnSelect EventHandlers

		/// <summary>
		/// Occurs before the ExpandTreeNodeOnSelect property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs before the ExpandTreeNodeOnSelect property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeExpandTreeNodeOnSelectChanged;

		/// <summary>
		/// Raises the BeforeExpandTreeNodeOnSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeExpandTreeNodeOnSelectChanged(MWCancelEventArgs e)
		{
			if(BeforeExpandTreeNodeOnSelectChanged != null)
			{
				BeforeExpandTreeNodeOnSelectChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ExpandTreeNodeOnSelect property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Expanding/Collapsing"),
		Description("Occurs after the ExpandTreeNodeOnSelect property has changed.")
		]
		public event EventHandler AfterExpandTreeNodeOnSelectChanged;

		/// <summary>
		/// Raises the AfterExpandTreeNodeOnSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterExpandTreeNodeOnSelectChanged(EventArgs e)
		{
			if(AfterExpandTreeNodeOnSelectChanged != null)
			{
				AfterExpandTreeNodeOnSelectChanged(this, e);
			}
		}

		#endregion ExpandTreeNodeOnSelect EventHandlers

		#endregion ExpandTreeNodeOnSelect Property and EventHandlers

		#endregion Category 'Behavior, MW - Expanding/Collapsing' Properties



		#region Category 'Behavior, MW - Keys' Properties

		#region AllowControlKeyNavigation Property and EventHandlers

		#region AllowControlKeyNavigation Property

		/// <summary>
		/// True to allow the navigation/scrolling that is achieved by the normal TreeView by holding down the Control key and then pressing the Down, Up, Right, Left, Home, End, PageUp or PageDown keys or false to disallow this.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow the navigation/scrolling that is achieved by the normal TreeView by holding down the Control key and then pressing the Down, Up, Right, Left, Home, End, PageUp or PageDown keys or false to disallow this."),
		DefaultValue(true)
		]
		public bool AllowControlKeyNavigation
		{
			get
			{
				return bAllowControlKeyNavigation;
			}
			set
			{
				if(bAllowControlKeyNavigation != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowControlKeyNavigation, value);
					OnBeforeAllowControlKeyNavigationChanged(e);

					if(!e.Cancel)
					{
						bAllowControlKeyNavigation = value;

						OnAfterAllowControlKeyNavigationChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowControlKeyNavigation Property



		#region AllowControlKeyNavigation EventHandlers

		/// <summary>
		/// Occurs before the AllowControlKeyNavigation property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowControlKeyNavigation property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowControlKeyNavigationChanged;

		/// <summary>
		/// Raises the BeforeAllowControlKeyNavigationChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowControlKeyNavigationChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowControlKeyNavigationChanged != null)
			{
				BeforeAllowControlKeyNavigationChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowControlKeyNavigation property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowControlKeyNavigation property has changed.")
		]
		public event EventHandler AfterAllowControlKeyNavigationChanged;

		/// <summary>
		/// Raises the AfterAllowControlKeyNavigationChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowControlKeyNavigationChanged(EventArgs e)
		{
			if(AfterAllowControlKeyNavigationChanged != null)
			{
				AfterAllowControlKeyNavigationChanged(this, e);
			}
		}

		#endregion AllowControlKeyNavigation EventHandlers

		#endregion AllowControlKeyNavigation Property and EventHandlers



		#region AllowKeyASelectAll Property and EventHandlers

		#region AllowKeyASelectAll Property

		/// <summary>
		/// True if pressing the A-key should select all TreeNodes or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True if pressing the A-key should select all TreeNodes or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyASelectAll
		{
			get
			{
				return bAllowKeyASelectAll;
			}
			set
			{
				if(bAllowKeyASelectAll != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyASelectAll, value);
					OnBeforeAllowKeyASelectAllChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyASelectAll = value;

						OnAfterAllowKeyASelectAllChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyASelectAll Property



		#region AllowKeyASelectAll EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyASelectAll property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyASelectAll property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyASelectAllChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyASelectAllChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyASelectAllChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyASelectAllChanged != null)
			{
				BeforeAllowKeyASelectAllChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyASelectAll property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyASelectAll property has changed.")
		]
		public event EventHandler AfterAllowKeyASelectAllChanged;

		/// <summary>
		/// Raises the AfterAllowKeyASelectAllChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyASelectAllChanged(EventArgs e)
		{
			if(AfterAllowKeyASelectAllChanged != null)
			{
				AfterAllowKeyASelectAllChanged(this, e);
			}
		}

		#endregion AllowKeyASelectAll EventHandlers

		#endregion AllowKeyASelectAll Property and EventHandlers



		#region AllowKeyEExpandAll Property and EventHandlers

		#region AllowKeyEExpandAll Property

		/// <summary>
		/// True if pressing the E-key should expand all TreeNodes or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True if pressing the E-key should expand all TreeNodes or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyEExpandAll
		{
			get
			{
				return bAllowKeyEExpandAll;
			}
			set
			{
				if(bAllowKeyEExpandAll != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyEExpandAll, value);
					OnBeforeAllowKeyEExpandAllChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyEExpandAll = value;

						OnAfterAllowKeyEExpandAllChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyEExpandAll Property



		#region AllowKeyEExpandAll EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyEExpandAll property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyEExpandAll property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyEExpandAllChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyEExpandAllChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyEExpandAllChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyEExpandAllChanged != null)
			{
				BeforeAllowKeyEExpandAllChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyEExpandAll property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyEExpandAll property has changed.")
		]
		public event EventHandler AfterAllowKeyEExpandAllChanged;

		/// <summary>
		/// Raises the AfterAllowKeyEExpandAllChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyEExpandAllChanged(EventArgs e)
		{
			if(AfterAllowKeyEExpandAllChanged != null)
			{
				AfterAllowKeyEExpandAllChanged(this, e);
			}
		}

		#endregion AllowKeyEExpandAll EventHandlers

		#endregion AllowKeyEExpandAll Property and EventHandlers



		#region AllowKeyEscapeDeselectAll Property and EventHandlers

		#region AllowKeyEscapeDeselectAll Property

		/// <summary>
		/// True if pressing the escape-key should deselect all selected TreeNodes or false otherwise (escape is still used for exiting LabelEdit/NodeEdit).
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True if pressing the escape-key should deselect all selected TreeNodes or false otherwise (escape is still used for exiting LabelEdit/NodeEdit)."),
		DefaultValue(true)
		]
		public bool AllowKeyEscapeDeselectAll
		{
			get
			{
				return bAllowKeyEscapeDeselectAll;
			}
			set
			{
				if(bAllowKeyEscapeDeselectAll != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyEscapeDeselectAll, value);
					OnBeforeAllowKeyEscapeDeselectAllChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyEscapeDeselectAll = value;

						OnAfterAllowKeyEscapeDeselectAllChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyEscapeDeselectAll Property



		#region AllowKeyEscapeDeselectAll EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyEscapeDeselectAll property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyEscapeDeselectAll property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyEscapeDeselectAllChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyEscapeDeselectAllChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyEscapeDeselectAllChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyEscapeDeselectAllChanged != null)
			{
				BeforeAllowKeyEscapeDeselectAllChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyEscapeDeselectAll property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyEscapeDeselectAll property has changed.")
		]
		public event EventHandler AfterAllowKeyEscapeDeselectAllChanged;

		/// <summary>
		/// Raises the AfterAllowKeyEscapeDeselectAllChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyEscapeDeselectAllChanged(EventArgs e)
		{
			if(AfterAllowKeyEscapeDeselectAllChanged != null)
			{
				AfterAllowKeyEscapeDeselectAllChanged(this, e);
			}
		}

		#endregion AllowKeyEscapeDeselectAll EventHandlers

		#endregion AllowKeyEscapeDeselectAll Property and EventHandlers



		#region AllowKeyF2LabelEditing Property and EventHandlers

		#region AllowKeyF2LabelEditing Property

		/// <summary>
		/// True if label editing starts by pressing the F2-key or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True if label editing starts by pressing the F2-key or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyF2LabelEditing
		{
			get
			{
				return bAllowKeyF2LabelEditing;
			}
			set
			{
				if(bAllowKeyF2LabelEditing != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyF2LabelEditing, value);
					OnBeforeAllowKeyF2LabelEditingChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyF2LabelEditing = value;

						OnAfterAllowKeyF2LabelEditingChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyF2LabelEditing Property



		#region AllowKeyF2LabelEditing EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyF2LabelEditing property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyF2LabelEditing property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyF2LabelEditingChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyF2LabelEditingChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyF2LabelEditingChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyF2LabelEditingChanged != null)
			{
				BeforeAllowKeyF2LabelEditingChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyF2LabelEditing property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyF2LabelEditing property has changed.")
		]
		public event EventHandler AfterAllowKeyF2LabelEditingChanged;

		/// <summary>
		/// Raises the AfterAllowKeyF2LabelEditingChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyF2LabelEditingChanged(EventArgs e)
		{
			if(AfterAllowKeyF2LabelEditingChanged != null)
			{
				AfterAllowKeyF2LabelEditingChanged(this, e);
			}
		}

		#endregion AllowKeyF2LabelEditing EventHandlers

		#endregion AllowKeyF2LabelEditing Property and EventHandlers



		#region AllowKeyQCollapseAll Property and EventHandlers

		#region AllowKeyQCollapseAll Property

		/// <summary>
		/// True if pressing the Q-key should collapse all TreeNodes or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True if pressing the Q-key should collapse all TreeNodes or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyQCollapseAll
		{
			get
			{
				return bAllowKeyQCollapseAll;
			}
			set
			{
				if(bAllowKeyQCollapseAll != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyQCollapseAll, value);
					OnBeforeAllowKeyQCollapseAllChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyQCollapseAll = value;

						OnAfterAllowKeyQCollapseAllChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyQCollapseAll Property



		#region AllowKeyQCollapseAll EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyQCollapseAll property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyQCollapseAll property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyQCollapseAllChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyQCollapseAllChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyQCollapseAllChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyQCollapseAllChanged != null)
			{
				BeforeAllowKeyQCollapseAllChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyQCollapseAll property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyQCollapseAll property has changed.")
		]
		public event EventHandler AfterAllowKeyQCollapseAllChanged;

		/// <summary>
		/// Raises the AfterAllowKeyQCollapseAllChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyQCollapseAllChanged(EventArgs e)
		{
			if(AfterAllowKeyQCollapseAllChanged != null)
			{
				AfterAllowKeyQCollapseAllChanged(this, e);
			}
		}

		#endregion AllowKeyQCollapseAll EventHandlers

		#endregion AllowKeyQCollapseAll Property and EventHandlers

		#endregion Category 'Behavior, MW - Keys' Properties



		#region Category 'Behavior, MW - Keys, Modifier Keys' Properties

		#region AllowKeyboardModKeyAlt Property and EventHandlers

		#region AllowKeyboardModKeyAlt Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyboardModKeyAlt
		{
			get
			{
				return bAllowKeyboardModKeyAlt;
			}
			set
			{
				if(bAllowKeyboardModKeyAlt != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyboardModKeyAlt, value);
					OnBeforeAllowKeyboardModKeyAltChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyboardModKeyAlt = value;

						OnAfterAllowKeyboardModKeyAltChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyboardModKeyAlt Property



		#region AllowKeyboardModKeyAlt EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyboardModKeyAlt property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyboardModKeyAlt property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyboardModKeyAltChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyboardModKeyAltChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyboardModKeyAltChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyboardModKeyAltChanged != null)
			{
				BeforeAllowKeyboardModKeyAltChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyboardModKeyAlt property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyboardModKeyAlt property has changed.")
		]
		public event EventHandler AfterAllowKeyboardModKeyAltChanged;

		/// <summary>
		/// Raises the AfterAllowKeyboardModKeyAltChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyboardModKeyAltChanged(EventArgs e)
		{
			if(AfterAllowKeyboardModKeyAltChanged != null)
			{
				AfterAllowKeyboardModKeyAltChanged(this, e);
			}
		}

		#endregion AllowKeyboardModKeyAlt EventHandlers

		#endregion AllowKeyboardModKeyAlt Property and EventHandlers



		#region AllowKeyboardModKeyControl Property and EventHandlers

		#region AllowKeyboardModKeyControl Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyboardModKeyControl
		{
			get
			{
				return bAllowKeyboardModKeyControl;
			}
			set
			{
				if(bAllowKeyboardModKeyControl != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyboardModKeyControl, value);
					OnBeforeAllowKeyboardModKeyControlChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyboardModKeyControl = value;

						OnAfterAllowKeyboardModKeyControlChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyboardModKeyControl Property



		#region AllowKeyboardModKeyControl EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyboardModKeyControl property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyboardModKeyControl property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyboardModKeyControlChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyboardModKeyControlChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyboardModKeyControlChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyboardModKeyControlChanged != null)
			{
				BeforeAllowKeyboardModKeyControlChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyboardModKeyControl property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyboardModKeyControl property has changed.")
		]
		public event EventHandler AfterAllowKeyboardModKeyControlChanged;

		/// <summary>
		/// Raises the AfterAllowKeyboardModKeyControlChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyboardModKeyControlChanged(EventArgs e)
		{
			if(AfterAllowKeyboardModKeyControlChanged != null)
			{
				AfterAllowKeyboardModKeyControlChanged(this, e);
			}
		}

		#endregion AllowKeyboardModKeyControl EventHandlers

		#endregion AllowKeyboardModKeyControl Property and EventHandlers



		#region AllowKeyboardModKeyShift Property and EventHandlers

		#region AllowKeyboardModKeyShift Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowKeyboardModKeyShift
		{
			get
			{
				return bAllowKeyboardModKeyShift;
			}
			set
			{
				if(bAllowKeyboardModKeyShift != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowKeyboardModKeyShift, value);
					OnBeforeAllowKeyboardModKeyShiftChanged(e);

					if(!e.Cancel)
					{
						bAllowKeyboardModKeyShift = value;

						OnAfterAllowKeyboardModKeyShiftChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowKeyboardModKeyShift Property



		#region AllowKeyboardModKeyShift EventHandlers

		/// <summary>
		/// Occurs before the AllowKeyboardModKeyShift property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowKeyboardModKeyShift property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowKeyboardModKeyShiftChanged;

		/// <summary>
		/// Raises the BeforeAllowKeyboardModKeyShiftChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowKeyboardModKeyShiftChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowKeyboardModKeyShiftChanged != null)
			{
				BeforeAllowKeyboardModKeyShiftChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowKeyboardModKeyShift property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowKeyboardModKeyShift property has changed.")
		]
		public event EventHandler AfterAllowKeyboardModKeyShiftChanged;

		/// <summary>
		/// Raises the AfterAllowKeyboardModKeyShiftChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowKeyboardModKeyShiftChanged(EventArgs e)
		{
			if(AfterAllowKeyboardModKeyShiftChanged != null)
			{
				AfterAllowKeyboardModKeyShiftChanged(this, e);
			}
		}

		#endregion AllowKeyboardModKeyShift EventHandlers

		#endregion AllowKeyboardModKeyShift Property and EventHandlers



		#region AllowMouseModKeyAlt Property and EventHandlers

		#region AllowMouseModKeyAlt Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Alt-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowMouseModKeyAlt
		{
			get
			{
				return bAllowMouseModKeyAlt;
			}
			set
			{
				if(bAllowMouseModKeyAlt != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowMouseModKeyAlt, value);
					OnBeforeAllowMouseModKeyAltChanged(e);

					if(!e.Cancel)
					{
						bAllowMouseModKeyAlt = value;

						OnAfterAllowMouseModKeyAltChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowMouseModKeyAlt Property



		#region AllowMouseModKeyAlt EventHandlers

		/// <summary>
		/// Occurs before the AllowMouseModKeyAlt property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowMouseModKeyAlt property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowMouseModKeyAltChanged;

		/// <summary>
		/// Raises the BeforeAllowMouseModKeyAltChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowMouseModKeyAltChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowMouseModKeyAltChanged != null)
			{
				BeforeAllowMouseModKeyAltChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowMouseModKeyAlt property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowMouseModKeyAlt property has changed.")
		]
		public event EventHandler AfterAllowMouseModKeyAltChanged;

		/// <summary>
		/// Raises the AfterAllowMouseModKeyAltChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowMouseModKeyAltChanged(EventArgs e)
		{
			if(AfterAllowMouseModKeyAltChanged != null)
			{
				AfterAllowMouseModKeyAltChanged(this, e);
			}
		}

		#endregion AllowMouseModKeyAlt EventHandlers

		#endregion AllowMouseModKeyAlt Property and EventHandlers



		#region AllowMouseModKeyControl Property and EventHandlers

		#region AllowMouseModKeyControl Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Control-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowMouseModKeyControl
		{
			get
			{
				return bAllowMouseModKeyControl;
			}
			set
			{
				if(bAllowMouseModKeyControl != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowMouseModKeyControl, value);
					OnBeforeAllowMouseModKeyControlChanged(e);

					if(!e.Cancel)
					{
						bAllowMouseModKeyControl = value;

						OnAfterAllowMouseModKeyControlChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowMouseModKeyControl Property



		#region AllowMouseModKeyControl EventHandlers

		/// <summary>
		/// Occurs before the AllowMouseModKeyControl property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowMouseModKeyControl property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowMouseModKeyControlChanged;

		/// <summary>
		/// Raises the BeforeAllowMouseModKeyControlChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowMouseModKeyControlChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowMouseModKeyControlChanged != null)
			{
				BeforeAllowMouseModKeyControlChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowMouseModKeyControl property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowMouseModKeyControl property has changed.")
		]
		public event EventHandler AfterAllowMouseModKeyControlChanged;

		/// <summary>
		/// Raises the AfterAllowMouseModKeyControlChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowMouseModKeyControlChanged(EventArgs e)
		{
			if(AfterAllowMouseModKeyControlChanged != null)
			{
				AfterAllowMouseModKeyControlChanged(this, e);
			}
		}

		#endregion AllowMouseModKeyControl EventHandlers

		#endregion AllowMouseModKeyControl Property and EventHandlers



		#region AllowMouseModKeyShift Property and EventHandlers

		#region AllowMouseModKeyShift Property

		/// <summary>
		/// True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("True to allow MWTreeView enhancements triggered by the modifier Shift-Key to work or false otherwise."),
		DefaultValue(true)
		]
		public bool AllowMouseModKeyShift
		{
			get
			{
				return bAllowMouseModKeyShift;
			}
			set
			{
				if(bAllowMouseModKeyShift != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowMouseModKeyShift, value);
					OnBeforeAllowMouseModKeyShiftChanged(e);

					if(!e.Cancel)
					{
						bAllowMouseModKeyShift = value;

						OnAfterAllowMouseModKeyShiftChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowMouseModKeyShift Property



		#region AllowMouseModKeyShift EventHandlers

		/// <summary>
		/// Occurs before the AllowMouseModKeyShift property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs before the AllowMouseModKeyShift property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowMouseModKeyShiftChanged;

		/// <summary>
		/// Raises the BeforeAllowMouseModKeyShiftChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowMouseModKeyShiftChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowMouseModKeyShiftChanged != null)
			{
				BeforeAllowMouseModKeyShiftChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowMouseModKeyShift property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Keys"),
		Description("Occurs after the AllowMouseModKeyShift property has changed.")
		]
		public event EventHandler AfterAllowMouseModKeyShiftChanged;

		/// <summary>
		/// Raises the AfterAllowMouseModKeyShiftChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowMouseModKeyShiftChanged(EventArgs e)
		{
			if(AfterAllowMouseModKeyShiftChanged != null)
			{
				AfterAllowMouseModKeyShiftChanged(this, e);
			}
		}

		#endregion AllowMouseModKeyShift EventHandlers

		#endregion AllowMouseModKeyShift Property and EventHandlers

		#endregion Category 'Behavior, MW - Keys, Modifier Keys' Properties



		#region Category 'Behavior, MW - Label-Editing' Properties

		#region AllowBlankNodeText Property

		#region AllowBlankNodeText Property and EventHandlers

		/// <summary>
		/// True if TreeNodes can be blank, i.e. contain no Text or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("True if TreeNodes can be blank, i.e. contain no Text or false otherwise."),
		DefaultValue(false)
		]
		public bool AllowBlankNodeText
		{
			get
			{
				return bAllowBlankNodeText;
			}
			set
			{
				if(bAllowBlankNodeText != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowBlankNodeText, value);
					OnBeforeAllowBlankNodeTextChanged(e);

					if(!e.Cancel)
					{
						bAllowBlankNodeText = value;

						OnAfterAllowBlankNodeTextChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowBlankNodeText Property



		#region AllowBlankNodeText EventHandlers

		/// <summary>
		/// Occurs before the AllowBlankNodeText property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs before the AllowBlankNodeText property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowBlankNodeTextChanged;

		/// <summary>
		/// Raises the BeforeAllowBlankNodeTextChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowBlankNodeTextChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowBlankNodeTextChanged != null)
			{
				BeforeAllowBlankNodeTextChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowBlankNodeText property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs after the AllowBlankNodeText property has changed.")
		]
		public event EventHandler AfterAllowBlankNodeTextChanged;

		/// <summary>
		/// Raises the AfterAllowBlankNodeTextChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowBlankNodeTextChanged(EventArgs e)
		{
			if(AfterAllowBlankNodeTextChanged != null)
			{
				AfterAllowBlankNodeTextChanged(this, e);
			}
		}

		#endregion AllowBlankNodeText EventHandlers

		#endregion AllowBlankNodeText Property and EventHandlers



		#region DisallowLabelEditRegEx Property and EventHandlers

		#region DisallowLabelEditRegEx Property

		/// <summary>
		/// Regular expression that cannot be satisfied if the Text of a TreeNode should be able to be changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Regular expression that cannot be satisfied if the Text of a TreeNode should be able to be changed."),
		DefaultValue("")
		]
		public string DisallowLabelEditRegEx
		{
			get
			{
				return strDisallowLabelEditRegEx;
			}
			set
			{
				if(strDisallowLabelEditRegEx != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(strDisallowLabelEditRegEx, value);
					OnBeforeDisallowLabelEditRegExChanged(e);

					if(!e.Cancel)
					{
						strDisallowLabelEditRegEx = value;

						OnAfterDisallowLabelEditRegExChanged(new EventArgs());
					}
				}
			}
		}

		#endregion DisallowLabelEditRegEx Property



		#region DisallowLabelEditRegEx EventHandlers

		/// <summary>
		/// Occurs before the DisallowLabelEditRegEx property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs before the DisallowLabelEditRegEx property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeDisallowLabelEditRegExChanged;

		/// <summary>
		/// Raises the BeforeDisallowLabelEditRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeDisallowLabelEditRegExChanged(MWCancelEventArgs e)
		{
			if(BeforeDisallowLabelEditRegExChanged != null)
			{
				BeforeDisallowLabelEditRegExChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the DisallowLabelEditRegEx property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs after the DisallowLabelEditRegEx property has changed.")
		]
		public event EventHandler AfterDisallowLabelEditRegExChanged;

		/// <summary>
		/// Raises the AfterDisallowLabelEditRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterDisallowLabelEditRegExChanged(EventArgs e)
		{
			if(AfterDisallowLabelEditRegExChanged != null)
			{
				AfterDisallowLabelEditRegExChanged(this, e);
			}
		}

		#endregion DisallowLabelEditRegEx EventHandlers

		#endregion DisallowLabelEditRegEx Property and EventHandlers



		#region LabelEditRegEx Property and EventHandlers

		#region LabelEditRegEx Property

		/// <summary>
		/// Regular expression that has to be satisfied before the Text of a TreeNode can be changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Regular expression that has to be satisfied before the Text of a TreeNode can be changed."),
		DefaultValue("")
		]
		public string LabelEditRegEx
		{
			get
			{
				return strLabelEditRegEx;
			}
			set
			{
				if(strLabelEditRegEx != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(strLabelEditRegEx, value);
					OnBeforeLabelEditRegExChanged(e);

					if(!e.Cancel)
					{
						strLabelEditRegEx = value;

						OnAfterLabelEditRegExChanged(new EventArgs());
					}
				}
			}
		}

		#endregion LabelEditRegEx Property



		#region LabelEditRegEx EventHandlers

		/// <summary>
		/// Occurs before the LabelEditRegEx property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs before the LabelEditRegEx property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeLabelEditRegExChanged;

		/// <summary>
		/// Raises the BeforeLabelEditRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeLabelEditRegExChanged(MWCancelEventArgs e)
		{
			if(BeforeLabelEditRegExChanged != null)
			{
				BeforeLabelEditRegExChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the LabelEditRegEx property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs after the LabelEditRegEx property has changed.")
		]
		public event EventHandler AfterLabelEditRegExChanged;

		/// <summary>
		/// Raises the AfterLabelEditRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterLabelEditRegExChanged(EventArgs e)
		{
			if(AfterLabelEditRegExChanged != null)
			{
				AfterLabelEditRegExChanged(this, e);
			}
		}

		#endregion LabelEditRegEx EventHandlers

		#endregion LabelEditRegEx Property and EventHandlers



		#region LabelEditTimeout Property and EventHandlers

		#region LabelEditTimeout Property

		/// <summary>
		/// Timeout (ms) before a TreeNode is put into LabelEdit/NodeEdit mode.
		/// Default-value: 200
		/// Min-value: 0
		/// Max-value: 10000
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Timeout (ms) before a TreeNode is put into LabelEdit/NodeEdit mode.\nDefault-value: 200\nMin-value: 0\nMax-value: 10000"),
		DefaultValue(200)
		]
		public int LabelEditTimeout
		{
			get
			{
				return iLabelEditTimeout;
			}
			set
			{
				if(	LabelEditTimeout != value &&
					value >= 0 &&
					value <= 10000)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iLabelEditTimeout, value);
					OnBeforeLabelEditTimeoutChanged(e);

					if(!e.Cancel)
					{
						iLabelEditTimeout = value;

						tLabelEdit.Interval = iLabelEditTimeout;
						tLabelEditAllowed.Interval = iLabelEditTimeout;

						OnAfterLabelEditTimeoutChanged(new EventArgs());
					}
				}
			}
		}

		#endregion LabelEditTimeout Property



		#region LabelEditTimeout EventHandlers

		/// <summary>
		/// Occurs before the LabelEditTimeout property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs before the LabelEditTimeout property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeLabelEditTimeoutChanged;

		/// <summary>
		/// Raises the BeforeLabelEditTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeLabelEditTimeoutChanged(MWCancelEventArgs e)
		{
			if(BeforeLabelEditTimeoutChanged != null)
			{
				BeforeLabelEditTimeoutChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the LabelEditTimeout property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Label Editing"),
		Description("Occurs after the LabelEditTimeout property has changed.")
		]
		public event EventHandler AfterLabelEditTimeoutChanged;

		/// <summary>
		/// Raises the AfterLabelEditTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterLabelEditTimeoutChanged(EventArgs e)
		{
			if(AfterLabelEditTimeoutChanged != null)
			{
				AfterLabelEditTimeoutChanged(this, e);
			}
		}

		#endregion LabelEditTimeout EventHandlers

		#endregion LabelEditTimeout Property and EventHandlers

		#endregion Category 'Behavior, MW - Label-Editing' Properties



		#region Category 'Behavior, MW - Mouse-Selection' Properties

		#region MouseClearSelNodes Property and EventHandlers

		#region MouseClearSelNodes Property

		/// <summary>
		/// Decides whether clicking on a blank area should deselect all TreeNodes or not.
		/// What constitutes a blank area is decided by the MouseClearSelNodes enum.
		/// Note that some of the MouseClearSelNodes enum members or part of what they represent are ignored if the FullRowSelect property is set to true.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("Decides whether clicking on a blank area should deselect all TreeNodes or not.\nWhat constitutes a blank area is decided by the MouseClearSelNodes enum.Note that some of the MouseClearSelNodes enum members or part of what they represent are ignored if the FullRowSelect property is set to true."),
		DefaultValue(typeof(MouseClearSelNodes), "None")
		]
		public MouseClearSelNodes MouseClearSelNodes
		{
			get
			{
				return mcsnMouseClearSelNodes;
			}
			set
			{
				if(mcsnMouseClearSelNodes != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(mcsnMouseClearSelNodes, value);
					OnBeforeMouseClearSelNodesChanged(e);

					if(!e.Cancel)
					{
						mcsnMouseClearSelNodes = value;

						OnAfterMouseClearSelNodesChanged(new EventArgs());
					}
				}
			}
		}

		#endregion MouseClearSelNodes Property



		#region MouseClearSelNodes EventHandlers

		/// <summary>
		/// Occurs before the MouseClearSelNodes property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("Occurs before the MouseClearSelNodes property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeMouseClearSelNodesChanged;

		/// <summary>
		/// Raises the BeforeMouseClearSelNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeMouseClearSelNodesChanged(MWCancelEventArgs e)
		{
			if(BeforeMouseClearSelNodesChanged != null)
			{
				BeforeMouseClearSelNodesChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the MouseClearSelNodes property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("Occurs after the MouseClearSelNodes property has changed.")
		]
		public event EventHandler AfterMouseClearSelNodesChanged;

		/// <summary>
		/// Raises the AfterMouseClearSelNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterMouseClearSelNodesChanged(EventArgs e)
		{
			if(AfterMouseClearSelNodesChanged != null)
			{
				AfterMouseClearSelNodesChanged(this, e);
			}
		}

		#endregion MouseClearSelNodes EventHandlers

		#endregion MouseClearSelNodes Property and EventHandlers



		#region ConstrainedShiftKeySelection Property and EventHandlers

		#region ConstrainedShiftKeySelection Property

		/// <summary>
		/// True if key selection is constrained by the starting TreeNode when shift-selecting or false otherwise.
		/// This is used for the following MultiSelect modes:
		///		MultiSameLevel
		///		MultiSameBranch
		///		MultiSameBranchAndLevel
		/// Tip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("True if key selection is constrained by the starting TreeNode when shift-selecting or false otherwise.\nThis is used for the following MultiSelect modes: MultiSameLevel, MultiSameBranch and MultiSameBranchAndLevel.\nTip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel."),
		DefaultValue(true)
		]
		public bool ConstrainedShiftKeySelection
		{
			get
			{
				return bConstrainedShiftKeySelection;
			}
			set
			{
				if(bConstrainedShiftKeySelection != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bConstrainedShiftKeySelection, value);
					OnBeforeConstrainedShiftKeySelectionChanged(e);

					if(!e.Cancel)
					{
						bConstrainedShiftKeySelection = value;

						OnAfterConstrainedShiftKeySelectionChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ConstrainedShiftKeySelection Property



		#region ConstrainedShiftKeySelection EventHandlers

		/// <summary>
		/// Occurs before the ConstrainedShiftKeySelection property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("Occurs before the ConstrainedShiftKeySelection property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeConstrainedShiftKeySelectionChanged;

		/// <summary>
		/// Raises the BeforeConstrainedShiftKeySelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeConstrainedShiftKeySelectionChanged(MWCancelEventArgs e)
		{
			if(BeforeConstrainedShiftKeySelectionChanged != null)
			{
				BeforeConstrainedShiftKeySelectionChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ConstrainedShiftKeySelection property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Mouse-Selection"),
		Description("Occurs after the ConstrainedShiftKeySelection property has changed.")
		]
		public event EventHandler AfterConstrainedShiftKeySelectionChanged;

		/// <summary>
		/// Raises the AfterConstrainedShiftKeySelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterConstrainedShiftKeySelectionChanged(EventArgs e)
		{
			if(AfterConstrainedShiftKeySelectionChanged != null)
			{
				AfterConstrainedShiftKeySelectionChanged(this, e);
			}
		}

		#endregion ConstrainedShiftKeySelection EventHandlers

		#endregion ConstrainedShiftKeySelection Property and EventHandlers

		#endregion Category 'Behavior, MW - Mouse-Selection' Properties



		#region Category 'Behavior, MW - Rubberband-Selection' Properties

		#region AllowOutOfFocusRubberbandSelection Property and EventHandlers

		#region AllowOutOfFocusRubberbandSelection Property

		/// <summary>
		/// True if rubberband selection should be allowed even though the MWTreeView, or the Form it is on, doesn't have focus or false otherwise.
		/// Note that setting this to false is probably more conventional and this is how ListViews etc work.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("True if rubberband selection should be allowed even though the MWTreeView, or the Form it is on, doesn't have focus or false otherwise.\nNote that setting this to false is probably more conventional and this is how ListViews etc work."),
		DefaultValue(false)
		]
		public bool AllowOutOfFocusRubberbandSelection
		{
			get
			{
				return bAllowOutOfFocusRubberbandSelection;
			}
			set
			{
				if(bAllowOutOfFocusRubberbandSelection != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowOutOfFocusRubberbandSelection, value);
					OnBeforeAllowOutOfFocusRubberbandSelectionChanged(e);

					if(!e.Cancel)
					{
						bAllowOutOfFocusRubberbandSelection = value;

						OnAfterAllowOutOfFocusRubberbandSelectionChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowOutOfFocusRubberbandSelection Property



		#region AllowOutOfFocusRubberbandSelection EventHandlers

		/// <summary>
		/// Occurs before the AllowOutOfFocusRubberbandSelection property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs before the AllowOutOfFocusRubberbandSelection property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowOutOfFocusRubberbandSelectionChanged;

		/// <summary>
		/// Raises the BeforeAllowOutOfFocusRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowOutOfFocusRubberbandSelectionChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowOutOfFocusRubberbandSelectionChanged != null)
			{
				BeforeAllowOutOfFocusRubberbandSelectionChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowOutOfFocusRubberbandSelection property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs after the AllowOutOfFocusRubberbandSelection property has changed.")
		]
		public event EventHandler AfterAllowOutOfFocusRubberbandSelectionChanged;

		/// <summary>
		/// Raises the AfterAllowOutOfFocusRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowOutOfFocusRubberbandSelectionChanged(EventArgs e)
		{
			if(AfterAllowOutOfFocusRubberbandSelectionChanged != null)
			{
				AfterAllowOutOfFocusRubberbandSelectionChanged(this, e);
			}
		}

		#endregion AllowOutOfFocusRubberbandSelection EventHandlers

		#endregion AllowOutOfFocusRubberbandSelection Property and EventHandlers



		#region ConstrainedRubberbandSelection Property and EventHandlers

		#region ConstrainedRubberbandSelection Property

		/// <summary>
		/// True if rubberband selection is constrained by the starting TreeNode of the rubberband selection or false otherwise.
		/// Tip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("True if rubberband selection is constrained by the starting TreeNode of the rubberband selection or false otherwise.\nTip: Test this property (true/false) with MultiSelect set to TreeViewMultiSelect.MultiSameLevel."),
		DefaultValue(true)
		]
		public bool ConstrainedRubberbandSelection
		{
			get
			{
				return bConstrainedRubberbandSelection;
			}
			set
			{
				if(bConstrainedRubberbandSelection != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bConstrainedRubberbandSelection, value);
					OnBeforeConstrainedRubberbandSelectionChanged(e);

					if(!e.Cancel)
					{
						bConstrainedRubberbandSelection = value;

						OnAfterConstrainedRubberbandSelectionChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ConstrainedRubberbandSelection Property



		#region ConstrainedRubberbandSelection EventHandlers

		/// <summary>
		/// Occurs before the ConstrainedRubberbandSelection property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs before the ConstrainedRubberbandSelection property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeConstrainedRubberbandSelectionChanged;

		/// <summary>
		/// Raises the BeforeConstrainedRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeConstrainedRubberbandSelectionChanged(MWCancelEventArgs e)
		{
			if(BeforeConstrainedRubberbandSelectionChanged != null)
			{
				BeforeConstrainedRubberbandSelectionChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ConstrainedRubberbandSelection property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs after the ConstrainedRubberbandSelection property has changed.")
		]
		public event EventHandler AfterConstrainedRubberbandSelectionChanged;

		/// <summary>
		/// Raises the AfterConstrainedRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterConstrainedRubberbandSelectionChanged(EventArgs e)
		{
			if(AfterConstrainedRubberbandSelectionChanged != null)
			{
				AfterConstrainedRubberbandSelectionChanged(this, e);
			}
		}

		#endregion ConstrainedRubberbandSelection EventHandlers

		#endregion ConstrainedRubberbandSelection Property and EventHandlers



		#region RubberbandSelection Property and EventHandlers

		#region RubberbandSelection Property

		/// <summary>
		/// Rubberband selection mode with which TreeNodes are selected.
		/// Note that if using a MultiSelect mode of TreeViewMultiSelect.NoMulti, this property is ignored.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Rubberband selection mode with which TreeNodes are selected.\nNote that if using a MultiSelect mode of TreeViewMultiSelect.NoMulti, this property is ignored."),
		DefaultValue(RubberbandSelectionMode.Fill)
		]
		public RubberbandSelectionMode RubberbandSelection
		{
			get
			{
				return rsmRubberbandSelection;
			}
			set
			{
				if(rsmRubberbandSelection != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(rsmRubberbandSelection, value);
					OnBeforeRubberbandSelectionChanged(e);

					if(!e.Cancel)
					{
						rsmRubberbandSelection = value;

						this.Invalidate(this.ClientRectangle, false);
						this.Update(true);

						OnAfterRubberbandSelectionChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandSelection Property



		#region RubberbandSelection EventHandlers

		/// <summary>
		/// Occurs before the RubberbandSelection property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs before the RubberbandSelection property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandSelectionChanged;

		/// <summary>
		/// Raises the BeforeRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandSelectionChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandSelectionChanged != null)
			{
				BeforeRubberbandSelectionChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandSelection property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs after the RubberbandSelection property has changed.")
		]
		public event EventHandler AfterRubberbandSelectionChanged;

		/// <summary>
		/// Raises the AfterRubberbandSelectionChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandSelectionChanged(EventArgs e)
		{
			if(AfterRubberbandSelectionChanged != null)
			{
				AfterRubberbandSelectionChanged(this, e);
			}
		}

		#endregion RubberbandSelection EventHandlers

		#endregion RubberbandSelection Property and EventHandlers



		#region RubberbandSelectionExpandCollapseTimeout Property and EventHandlers

		#region RubberbandSelectionExpandCollapseTimeout Property

		/// <summary>
		/// Timeout (ms) before rubberband selection is allowed after a TreeNode expand/collapse.
		/// Default-value: 1000
		/// Min-value: 0
		/// Max-value: 10000
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Timeout (ms) before rubberband selection is allowed after a TreeNode expand/collapse.\nDefault-value: 200\nMin-value: 0\nMax-value: 10000"),
		DefaultValue(1000)
		]
		public int RubberbandSelectionExpandCollapseTimeout
		{
			get
			{
				return iRubberbandSelectionExpandCollapseTimeout;
			}
			set
			{
				if(	RubberbandSelectionExpandCollapseTimeout != value &&
					value >= 0 &&
					value <= 10000)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandSelectionExpandCollapseTimeout, value);
					OnBeforeRubberbandSelectionExpandCollapseTimeoutChanged(e);

					if(!e.Cancel)
					{
						iRubberbandSelectionExpandCollapseTimeout = value;

						tRubberbandSelectionAllowedExpand.Interval = iRubberbandSelectionExpandCollapseTimeout;
						tRubberbandSelectionAllowedCollapse.Interval = iRubberbandSelectionExpandCollapseTimeout;

						OnAfterRubberbandSelectionExpandCollapseTimeoutChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandSelectionExpandCollapseTimeout Property



		#region RubberbandSelectionExpandCollapseTimeout EventHandlers

		/// <summary>
		/// Occurs before the RubberbandSelectionExpandCollapseTimeout property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs before the RubberbandSelectionExpandCollapseTimeout property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandSelectionExpandCollapseTimeoutChanged;

		/// <summary>
		/// Raises the BeforeRubberbandSelectionExpandCollapseTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandSelectionExpandCollapseTimeoutChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandSelectionExpandCollapseTimeoutChanged != null)
			{
				BeforeRubberbandSelectionExpandCollapseTimeoutChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandSelectionExpandCollapseTimeout property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection"),
		Description("Occurs after the RubberbandSelectionExpandCollapseTimeout property has changed.")
		]
		public event EventHandler AfterRubberbandSelectionExpandCollapseTimeoutChanged;

		/// <summary>
		/// Raises the AfterRubberbandSelectionExpandCollapseTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandSelectionExpandCollapseTimeoutChanged(EventArgs e)
		{
			if(AfterRubberbandSelectionExpandCollapseTimeoutChanged != null)
			{
				AfterRubberbandSelectionExpandCollapseTimeoutChanged(this, e);
			}
		}

		#endregion RubberbandSelectionExpandCollapseTimeout EventHandlers

		#endregion RubberbandSelectionExpandCollapseTimeout Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Border' Properties

		#region RubberbandBorderColor Property and EventHandlers

		#region RubberbandBorderColor Property

		/// <summary>
		/// Color that is used for the rubberband border.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Color that is used for the rubberband border."),
		DefaultValue(typeof(Color), "CornflowerBlue")
		]
		public Color RubberbandBorderColor
		{
			get
			{
				if(cRubberbandBorderColor.A != 255 || this.DesignMode)
				{
					return cRubberbandBorderColor;
				}
				else
				{
					return Color.FromArgb(iRubberbandBorderColorAlpha, cRubberbandBorderColor);
				}
			}
			set
			{
				if(cRubberbandBorderColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandBorderColor, value);
					OnBeforeRubberbandBorderColorChanged(e);

					if(!e.Cancel)
					{
						cRubberbandBorderColor = value;

						OnAfterRubberbandBorderColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandBorderColor Property



		#region RubberbandBorderColor EventHandlers

		/// <summary>
		/// Occurs before the RubberbandBorderColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs before the RubberbandBorderColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandBorderColorChanged;

		/// <summary>
		/// Raises the BeforeRubberbandBorderColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandBorderColorChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandBorderColorChanged != null)
			{
				BeforeRubberbandBorderColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandBorderColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs after the RubberbandBorderColor property has changed.")
		]
		public event EventHandler AfterRubberbandBorderColorChanged;

		/// <summary>
		/// Raises the AfterRubberbandBorderColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandBorderColorChanged(EventArgs e)
		{
			if(AfterRubberbandBorderColorChanged != null)
			{
				AfterRubberbandBorderColorChanged(this, e);
			}
		}

		#endregion RubberbandBorderColor EventHandlers

		#endregion RubberbandBorderColor Property and EventHandlers



		#region RubberbandBorderColorAlpha Property and EventHandlers

		#region RubberbandBorderColorAlpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandBorderColor if the RubberbandBorderColor has no alpha value set.
		///		I.e. it is used when RubberbandBorderColor is 100% opaque.
		///		If RubberbandBorderColor is translucent this variable is ignored.
		/// Default-value: 255
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Alpha channel value to be used with the RubberbandBorderColor if the RubberbandBorderColor has no alpha value set.\nI.e. it is used when RubberbandBorderColor is 100% opaque.\nIf RubberbandBorderColor is translucent this variable is ignored.\nDefault-value: 255\nMin-value: 0\nMax-value: 255"),
		DefaultValue(255)
		]
		public int RubberbandBorderColorAlpha
		{
			get
			{
				return iRubberbandBorderColorAlpha;
			}
			set
			{
				if(iRubberbandBorderColorAlpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandBorderColorAlpha, value);
					OnBeforeRubberbandBorderColorAlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandBorderColorAlpha = value;

						OnAfterRubberbandBorderColorAlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandBorderColorAlpha Property



		#region RubberbandBorderColorAlpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandBorderColorAlpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs before the RubberbandBorderColorAlpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandBorderColorAlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandBorderColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandBorderColorAlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandBorderColorAlphaChanged != null)
			{
				BeforeRubberbandBorderColorAlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandBorderColorAlpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs after the RubberbandBorderColorAlpha property has changed.")
		]
		public event EventHandler AfterRubberbandBorderColorAlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandBorderColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandBorderColorAlphaChanged(EventArgs e)
		{
			if(AfterRubberbandBorderColorAlphaChanged != null)
			{
				AfterRubberbandBorderColorAlphaChanged(this, e);
			}
		}

		#endregion RubberbandBorderColorAlpha EventHandlers

		#endregion RubberbandBorderColorAlpha Property and EventHandlers



		#region RubberbandBorderThickness Property and EventHandlers

		#region RubberbandBorderThickness Property

		/// <summary>
		/// The thickness of the rubberband border.
		/// Default-value: 1
		/// Min-value: 1
		/// Max-value: 32
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("The thickness of the rubberband border."),
		DefaultValue(1)
		]
		public int RubberbandBorderThickness
		{
			get
			{
				return iRubberbandBorderThickness;
			}
			set
			{
				if(iRubberbandBorderThickness != value &&
					value >= 1 &&
					value <= 32)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandBorderThickness, value);
					OnBeforeRubberbandBorderThicknessChanged(e);

					if(!e.Cancel)
					{
						iRubberbandBorderThickness = value;

						OnAfterRubberbandBorderThicknessChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandBorderThickness Property



		#region RubberbandBorderThickness EventHandlers

		/// <summary>
		/// Occurs before the RubberbandBorderThickness property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs before the RubberbandBorderThickness property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandBorderThicknessChanged;

		/// <summary>
		/// Raises the BeforeRubberbandBorderThicknessChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandBorderThicknessChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandBorderThicknessChanged != null)
			{
				BeforeRubberbandBorderThicknessChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandBorderThickness property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs after the RubberbandBorderThickness property has changed.")
		]
		public event EventHandler AfterRubberbandBorderThicknessChanged;

		/// <summary>
		/// Raises the AfterRubberbandBorderThicknessChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandBorderThicknessChanged(EventArgs e)
		{
			if(AfterRubberbandBorderThicknessChanged != null)
			{
				AfterRubberbandBorderThicknessChanged(this, e);
			}
		}

		#endregion RubberbandBorderThickness EventHandlers

		#endregion RubberbandBorderThickness Property and EventHandlers



		#region UseRubberbandBorder Property and EventHandlers

		#region UseRubberbandBorder Property

		/// <summary>
		/// True if a rubberband selection border should be used together with the RubberbandSelectionMode chosen.
		/// Note that this is ignored for the following RubberbandSelectionModes:
		///		RubberbandSelectionMode.None
		///		RubberbandSelectionMode.RubberbandDashed
		///		RubberbandSelectionMode.RubberbandSolid
		///		RubberbandSelectionMode.SelectionFrame
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("True if a rubberband selection border should be used together with the RubberbandSelectionMode chosen.\nNote that this is ignored for the following RubberbandSelectionModes:\n    RubberbandSelectionMode.None\n    RubberbandSelectionMode.RubberbandDashed\n    RubberbandSelectionMode.RubberbandSolid\n    RubberbandSelectionMode.SelectionFrame"),
		DefaultValue(true)
		]
		public bool UseRubberbandBorder
		{
			get
			{
				return bUseRubberbandBorder;
			}
			set
			{
				if(bUseRubberbandBorder != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bUseRubberbandBorder, value);
					OnBeforeUseRubberbandBorderChanged(e);

					if(!e.Cancel)
					{
						bUseRubberbandBorder = value;

						OnAfterUseRubberbandBorderChanged(new EventArgs());
					}
				}
			}
		}

		#endregion UseRubberbandBorder Property



		#region UseRubberbandBorder EventHandlers

		/// <summary>
		/// Occurs before the UseRubberbandBorder property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs before the UseRubberbandBorder property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeUseRubberbandBorderChanged;

		/// <summary>
		/// Raises the BeforeUseRubberbandBorderChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeUseRubberbandBorderChanged(MWCancelEventArgs e)
		{
			if(BeforeUseRubberbandBorderChanged != null)
			{
				BeforeUseRubberbandBorderChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the UseRubberbandBorder property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Border"),
		Description("Occurs after the UseRubberbandBorder property has changed.")
		]
		public event EventHandler AfterUseRubberbandBorderChanged;

		/// <summary>
		/// Raises the AfterUseRubberbandBorderChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterUseRubberbandBorderChanged(EventArgs e)
		{
			if(AfterUseRubberbandBorderChanged != null)
			{
				AfterUseRubberbandBorderChanged(this, e);
			}
		}

		#endregion UseRubberbandBorder EventHandlers

		#endregion UseRubberbandBorder Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Border' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Fill' Properties

		#region RubberbandFillColor Property and EventHandlers

		#region RubberbandFillColor Property

		/// <summary>
		/// Color that is used for rubberband Fill.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Color that is used for rubberband Fill."),
		DefaultValue(typeof(Color), "CornflowerBlue")
		]
		public Color RubberbandFillColor
		{
			get
			{
				if(cRubberbandFillColor.A != 255 || this.DesignMode)
				{
					return cRubberbandFillColor;
				}
				else
				{
					return Color.FromArgb(iRubberbandFillColorAlpha, cRubberbandFillColor);
				}
			}
			set
			{
				if(cRubberbandFillColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandFillColor, value);
					OnBeforeRubberbandFillColorChanged(e);

					if(!e.Cancel)
					{
						cRubberbandFillColor = value;

						OnAfterRubberbandFillColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandFillColor Property



		#region RubberbandFillColor EventHandlers

		/// <summary>
		/// Occurs before the RubberbandFillColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Occurs before the RubberbandFillColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandFillColorChanged;

		/// <summary>
		/// Raises the BeforeRubberbandFillColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandFillColorChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandFillColorChanged != null)
			{
				BeforeRubberbandFillColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandFillColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Occurs after the RubberbandFillColor property has changed.")
		]
		public event EventHandler AfterRubberbandFillColorChanged;

		/// <summary>
		/// Raises the AfterRubberbandFillColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandFillColorChanged(EventArgs e)
		{
			if(AfterRubberbandFillColorChanged != null)
			{
				AfterRubberbandFillColorChanged(this, e);
			}
		}

		#endregion RubberbandFillColor EventHandlers

		#endregion RubberbandFillColor Property and EventHandlers



		#region RubberbandFillColorAlpha Property and EventHandlers

		#region RubberbandFillColorAlpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandFillColor if the RubberbandFillColor has no alpha value set.
		///		I.e. it is used when RubberbandFillColor is 100% opaque.
		///		If RubberbandFillColor is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Alpha channel value to be used with the RubberbandFillColor if the RubberbandFillColor has no alpha value set.\nI.e. it is used when RubberbandFillColor is 100% opaque.\nIf RubberbandFillColor is translucent this variable is ignored.\nDefault-value: 128\nMin-value: 0\nMax-value: 255"),
		DefaultValue(128)
		]
		public int RubberbandFillColorAlpha
		{
			get
			{
				return iRubberbandFillColorAlpha;
			}
			set
			{
				if(iRubberbandFillColorAlpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandFillColorAlpha, value);
					OnBeforeRubberbandFillColorAlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandFillColorAlpha = value;

						OnAfterRubberbandFillColorAlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandFillColorAlpha Property



		#region RubberbandFillColorAlpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandFillColorAlpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Occurs before the RubberbandFillColorAlpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandFillColorAlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandFillColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandFillColorAlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandFillColorAlphaChanged != null)
			{
				BeforeRubberbandFillColorAlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandFillColorAlpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Fill"),
		Description("Occurs after the RubberbandFillColorAlpha property has changed.")
		]
		public event EventHandler AfterRubberbandFillColorAlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandFillColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandFillColorAlphaChanged(EventArgs e)
		{
			if(AfterRubberbandFillColorAlphaChanged != null)
			{
				AfterRubberbandFillColorAlphaChanged(this, e);
			}
		}

		#endregion RubberbandFillColorAlpha EventHandlers

		#endregion RubberbandFillColorAlpha Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Fill' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Gradient' Properties

		#region RubberbandGradientAngle Property and EventHandlers

		#region RubberbandGradientAngle Property

		/// <summary>
		/// Orientation angle that is used for rubberband Gradient.
		/// Note that this is only used if the GradientPaintMode property is set to Angle.
		/// Default-value: 45.0
		/// Min-value: -360.0
		/// Max-value: 360.0
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Orientation angle that is used for rubberband Gradient.\nNote that this is only used if the GradientPaintMode property is set to Angle.\nDefault-value: 45.0\nMin-value: -360.0\nMax-value: 360.0"),
		DefaultValue(45.0f)
		]
		public float RubberbandGradientAngle
		{
			get
			{
				return fRubberbandGradientAngle;
			}
			set
			{
				if(fRubberbandGradientAngle != value &&
					value >= -360.0f &&
					value <= 360.0f)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(fRubberbandGradientAngle, value);
					OnBeforeRubberbandGradientAngleChanged(e);

					if(!e.Cancel)
					{
						fRubberbandGradientAngle = value;

						OnAfterRubberbandGradientAngleChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientAngle Property



		#region RubberbandGradientAngle EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientAngle property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientAngle property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientAngleChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientAngleChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientAngleChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientAngleChanged != null)
			{
				BeforeRubberbandGradientAngleChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientAngle property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientAngle property has changed.")
		]
		public event EventHandler AfterRubberbandGradientAngleChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientAngleChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientAngleChanged(EventArgs e)
		{
			if(AfterRubberbandGradientAngleChanged != null)
			{
				AfterRubberbandGradientAngleChanged(this, e);
			}
		}

		#endregion RubberbandGradientAngle EventHandlers

		#endregion RubberbandGradientAngle Property and EventHandlers



		#region RubberbandGradientBlend Property and EventHandlers

		#region RubberbandGradientBlend Property

		/// <summary>
		/// Blend array that is used for rubberband Gradient.
		/// Note that this is only used when the RubberbandGradientShape property is set to GradientShape.Blend.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Blend array that is used for rubberband Gradient.\nNote that this is only used when the RubberbandGradientShape property is set to GradientShape.Blend."),
		DefaultValue(null)
		]
		public MWRubberbandGradientBlend[] RubberbandGradientBlend
		{
			get
			{
				return amwrgbRubberbandGradientBlend;
			}
			set
			{
				amwrgbRubberbandGradientBlend = value;

				//				if(amwrgbRubberbandGradientBlend != value)
				//				{
				//					MWCancelEventArgs e = new MWCancelEventArgs(amwrgbRubberbandGradientBlend, value);
				//					OnBeforeRubberbandGradientBlendChanged(e);
				//
				//					if(!e.Cancel)
				//					{
				//						amwrgbRubberbandGradientBlend = value;
				//
				//						OnAfterRubberbandGradientBlendChanged(new EventArgs());
				//					}
				//				}
			}
		}

		#endregion RubberbandGradientBlend Property



		#region RubberbandGradientBlend EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientBlend property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientBlend property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientBlendChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientBlendChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientBlendChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientBlendChanged != null)
			{
				BeforeRubberbandGradientBlendChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientBlend property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientBlend property has changed.")
		]
		public event EventHandler AfterRubberbandGradientBlendChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientBlendChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientBlendChanged(EventArgs e)
		{
			if(AfterRubberbandGradientBlendChanged != null)
			{
				AfterRubberbandGradientBlendChanged(this, e);
			}
		}

		#endregion RubberbandGradientBlend EventHandlers

		#endregion RubberbandGradientBlend Property and EventHandlers



		#region RubberbandGradientColorBlend Property and EventHandlers

		#region RubberbandGradientColorBlend Property

		/// <summary>
		/// ColorBlend array that is used for rubberband Gradient.
		/// Note that this is only used when the RubberbandGradientShape property is set to GradientShape.ColorBlend.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("ColorBlend array that is used for rubberband Gradient.\nNote that this is only used when the RubberbandGradientShape property is set to GradientShape.ColorBlend."),
		DefaultValue(null)
		]
		public MWRubberbandGradientColorBlend[] RubberbandGradientColorBlend
		{
			get
			{
				return amwrgcbRubberbandGradientColorBlend;
			}
			set
			{
				amwrgcbRubberbandGradientColorBlend = value;

				//				if(amwrgcbRubberbandGradientColorBlend != value)
				//				{
				//					MWCancelEventArgs e = new MWCancelEventArgs(amwrgcbRubberbandGradientColorBlend, value);
				//					OnBeforeRubberbandGradientColorBlendChanged(e);
				//
				//					if(!e.Cancel)
				//					{
				//						amwrgcbRubberbandGradientColorBlend = value;
				//
				//						OnAfterRubberbandGradientColorBlendChanged(new EventArgs());
				//					}
				//				}
			}
		}

		#endregion RubberbandGradientColorBlend Property



		#region RubberbandGradientColorBlend EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientColorBlend property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientColorBlend property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientColorBlendChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientColorBlendChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientColorBlendChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientColorBlendChanged != null)
			{
				BeforeRubberbandGradientColorBlendChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientColorBlend property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientColorBlend property has changed.")
		]
		public event EventHandler AfterRubberbandGradientColorBlendChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientColorBlendChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientColorBlendChanged(EventArgs e)
		{
			if(AfterRubberbandGradientColorBlendChanged != null)
			{
				AfterRubberbandGradientColorBlendChanged(this, e);
			}
		}

		#endregion RubberbandGradientColorBlend EventHandlers

		#endregion RubberbandGradientColorBlend Property and EventHandlers



		#region RubberbandGradientColor1 Property and EventHandlers

		#region RubberbandGradientColor1 Property

		/// <summary>
		/// Color 1 that is used for rubberband Gradient.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Color 1 that is used for rubberband Gradient."),
		DefaultValue(typeof(Color), "IndianRed")
		]
		public Color RubberbandGradientColor1
		{
			get
			{
				if(cRubberbandGradientColor1.A != 255 || this.DesignMode)
				{
					return cRubberbandGradientColor1;
				}
				else
				{
					return Color.FromArgb(iRubberbandGradientColor1Alpha, cRubberbandGradientColor1);
				}
			}
			set
			{
				if(cRubberbandGradientColor1 != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandGradientColor1, value);
					OnBeforeRubberbandGradientColor1Changed(e);

					if(!e.Cancel)
					{
						cRubberbandGradientColor1 = value;

						OnAfterRubberbandGradientColor1Changed(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientColor1 Property



		#region RubberbandGradientColor1 EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientColor1 property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientColor1 property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientColor1Changed;

		/// <summary>
		/// Raises the BeforeRubberbandGradientColor1Changed Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientColor1Changed(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientColor1Changed != null)
			{
				BeforeRubberbandGradientColor1Changed(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientColor1 property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientColor1 property has changed.")
		]
		public event EventHandler AfterRubberbandGradientColor1Changed;

		/// <summary>
		/// Raises the AfterRubberbandGradientColor1Changed Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientColor1Changed(EventArgs e)
		{
			if(AfterRubberbandGradientColor1Changed != null)
			{
				AfterRubberbandGradientColor1Changed(this, e);
			}
		}

		#endregion RubberbandGradientColor1 EventHandlers

		#endregion RubberbandGradientColor1 Property and EventHandlers



		#region RubberbandGradientColor1Alpha Property and EventHandlers

		#region RubberbandGradientColor1Alpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandGradientColor1 if the RubberbandGradientColor1 has no alpha value set.
		///		I.e. it is used when RubberbandGradientColor1 is 100% opaque.
		///		If RubberbandGradientColor1 is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Alpha channel value to be used with the RubberbandGradientColor1 if the RubberbandGradientColor1 has no alpha value set.\nI.e. it is used when RubberbandGradientColor1 is 100% opaque.\nIf RubberbandGradientColor1 is translucent this variable is ignored.\nDefault-value: 64\nMin-value: 0\nMax-value: 255"),
		DefaultValue(128)
		]
		public int RubberbandGradientColor1Alpha
		{
			get
			{
				return iRubberbandGradientColor1Alpha;
			}
			set
			{
				if(iRubberbandGradientColor1Alpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandGradientColor1Alpha, value);
					OnBeforeRubberbandGradientColor1AlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandGradientColor1Alpha = value;

						OnAfterRubberbandGradientColor1AlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientColor1Alpha Property



		#region RubberbandGradientColor1Alpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientColor1Alpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientColor1Alpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientColor1AlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientColor1AlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientColor1AlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientColor1AlphaChanged != null)
			{
				BeforeRubberbandGradientColor1AlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientColor1Alpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientColor1Alpha property has changed.")
		]
		public event EventHandler AfterRubberbandGradientColor1AlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientColor1AlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientColor1AlphaChanged(EventArgs e)
		{
			if(AfterRubberbandGradientColor1AlphaChanged != null)
			{
				AfterRubberbandGradientColor1AlphaChanged(this, e);
			}
		}

		#endregion RubberbandGradientColor1Alpha EventHandlers

		#endregion RubberbandGradientColor1Alpha Property and EventHandlers



		#region RubberbandGradientColor2 Property and EventHandlers

		#region RubberbandGradientColor2 Property

		/// <summary>
		/// Color 2 that is used for rubberband Gradient.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Color 2 that is used for rubberband Gradient."),
		DefaultValue(typeof(Color), "CornflowerBlue")
		]
		public Color RubberbandGradientColor2
		{
			get
			{
				if(cRubberbandGradientColor2.A != 255 || this.DesignMode)
				{
					return cRubberbandGradientColor2;
				}
				else
				{
					return Color.FromArgb(iRubberbandGradientColor2Alpha, cRubberbandGradientColor2);
				}
			}
			set
			{
				if(cRubberbandGradientColor2 != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandGradientColor2, value);
					OnBeforeRubberbandGradientColor2Changed(e);

					if(!e.Cancel)
					{
						cRubberbandGradientColor2 = value;

						OnAfterRubberbandGradientColor2Changed(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientColor2 Property



		#region RubberbandGradientColor2 EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientColor2 property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientColor2 property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientColor2Changed;

		/// <summary>
		/// Raises the BeforeRubberbandGradientColor2Changed Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientColor2Changed(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientColor2Changed != null)
			{
				BeforeRubberbandGradientColor2Changed(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientColor2 property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientColor2 property has changed.")
		]
		public event EventHandler AfterRubberbandGradientColor2Changed;

		/// <summary>
		/// Raises the AfterRubberbandGradientColor2Changed Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientColor2Changed(EventArgs e)
		{
			if(AfterRubberbandGradientColor2Changed != null)
			{
				AfterRubberbandGradientColor2Changed(this, e);
			}
		}

		#endregion RubberbandGradientColor2 EventHandlers

		#endregion RubberbandGradientColor2 Property and EventHandlers



		#region RubberbandGradientColor2Alpha Property and EventHandlers

		#region RubberbandGradientColor2Alpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandGradientColor2 if the RubberbandGradientColor2 has no alpha value set.
		///		I.e. it is used when RubberbandGradientColor2 is 100% opaque.
		///		If RubberbandGradientColor2 is translucent this variable is ignored.
		/// Default-value: 128
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Alpha channel value to be used with the RubberbandGradientColor2 if the RubberbandGradientColor2 has no alpha value set.\nI.e. it is used when RubberbandGradientColor2 is 100% opaque.\nIf RubberbandGradientColor2 is translucent this variable is ignored.\nDefault-value: 64\nMin-value: 0\nMax-value: 255"),
		DefaultValue(128)
		]
		public int RubberbandGradientColor2Alpha
		{
			get
			{
				return iRubberbandGradientColor2Alpha;
			}
			set
			{
				if(iRubberbandGradientColor2Alpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandGradientColor2Alpha, value);
					OnBeforeRubberbandGradientColor2AlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandGradientColor2Alpha = value;

						OnAfterRubberbandGradientColor2AlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientColor2Alpha Property



		#region RubberbandGradientColor2Alpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientColor2Alpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientColor2Alpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientColor2AlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientColor2AlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientColor2AlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientColor2AlphaChanged != null)
			{
				BeforeRubberbandGradientColor2AlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientColor2Alpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientColor2Alpha property has changed.")
		]
		public event EventHandler AfterRubberbandGradientColor2AlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientColor2AlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientColor2AlphaChanged(EventArgs e)
		{
			if(AfterRubberbandGradientColor2AlphaChanged != null)
			{
				AfterRubberbandGradientColor2AlphaChanged(this, e);
			}
		}

		#endregion RubberbandGradientColor2Alpha EventHandlers

		#endregion RubberbandGradientColor2Alpha Property and EventHandlers



		#region RubberbandGradientLinearGradientMode Property and EventHandlers

		#region RubberbandGradientLinearGradientMode Property

		/// <summary>
		/// LinearGradientMode that is used for rubberband Gradient.
		/// Note that this is only used if the GradientPaintMode property is set to LinearGradientMode.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("LinearGradientMode that is used for rubberband Gradient.\nNote that this is only used if the GradientPaintMode property is set to LinearGradientMode."),
		DefaultValue(typeof(LinearGradientMode), "ForwardDiagonal")
		]
		public LinearGradientMode RubberbandGradientLinearGradientMode
		{
			get
			{
				return lgmRubberbandGradientLinearGradientMode;
			}
			set
			{
				if(lgmRubberbandGradientLinearGradientMode != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(lgmRubberbandGradientLinearGradientMode, value);
					OnBeforeRubberbandGradientLinearGradientModeChanged(e);

					if(!e.Cancel)
					{
						lgmRubberbandGradientLinearGradientMode = value;

						OnAfterRubberbandGradientLinearGradientModeChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientLinearGradientMode Property



		#region RubberbandGradientLinearGradientMode EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientLinearGradientMode property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientLinearGradientMode property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientLinearGradientModeChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientLinearGradientModeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientLinearGradientModeChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientLinearGradientModeChanged != null)
			{
				BeforeRubberbandGradientLinearGradientModeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientLinearGradientMode property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientLinearGradientMode property has changed.")
		]
		public event EventHandler AfterRubberbandGradientLinearGradientModeChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientLinearGradientModeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientLinearGradientModeChanged(EventArgs e)
		{
			if(AfterRubberbandGradientLinearGradientModeChanged != null)
			{
				AfterRubberbandGradientLinearGradientModeChanged(this, e);
			}
		}

		#endregion RubberbandGradientLinearGradientMode EventHandlers

		#endregion RubberbandGradientLinearGradientMode Property and EventHandlers



		#region RubberbandGradientPaintMode Property and EventHandlers

		#region RubberbandGradientPaintMode Property

		/// <summary>
		/// GradientPaintMode that is used for rubberband Gradient.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("GradientPaintMode that is used for rubberband Gradient."),
		DefaultValue(typeof(GradientPaintMode), "MousePoints")
		]
		public GradientPaintMode RubberbandGradientPaintMode
		{
			get
			{
				return gpmRubberbandGradientPaintMode;
			}
			set
			{
				if(gpmRubberbandGradientPaintMode != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(gpmRubberbandGradientPaintMode, value);
					OnBeforeRubberbandGradientPaintModeChanged(e);

					if(!e.Cancel)
					{
						gpmRubberbandGradientPaintMode = value;

						OnAfterRubberbandGradientPaintModeChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientPaintMode Property



		#region RubberbandGradientPaintMode EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientPaintMode property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientPaintMode property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientPaintModeChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientPaintModeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientPaintModeChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientPaintModeChanged != null)
			{
				BeforeRubberbandGradientPaintModeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientPaintMode property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientPaintMode property has changed.")
		]
		public event EventHandler AfterRubberbandGradientPaintModeChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientPaintModeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientPaintModeChanged(EventArgs e)
		{
			if(AfterRubberbandGradientPaintModeChanged != null)
			{
				AfterRubberbandGradientPaintModeChanged(this, e);
			}
		}

		#endregion RubberbandGradientPaintMode EventHandlers

		#endregion RubberbandGradientPaintMode Property and EventHandlers



		#region RubberbandGradientShape Property and EventHandlers

		#region RubberbandGradientShape Property

		/// <summary>
		/// GradientShape that is used for rubberband Gradient.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("GradientShape that is used for rubberband Gradient."),
		DefaultValue(typeof(GradientShape), "Regular")
		]
		public GradientShape RubberbandGradientShape
		{
			get
			{
				return gsRubberbandGradientShape;
			}
			set
			{
				if(gsRubberbandGradientShape != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(gsRubberbandGradientShape, value);
					OnBeforeRubberbandGradientShapeChanged(e);

					if(!e.Cancel)
					{
						gsRubberbandGradientShape = value;

						OnAfterRubberbandGradientShapeChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientShape Property



		#region RubberbandGradientShape EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientShape property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientShape property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientShapeChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientShapeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientShapeChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientShapeChanged != null)
			{
				BeforeRubberbandGradientShapeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientShape property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientShape property has changed.")
		]
		public event EventHandler AfterRubberbandGradientShapeChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientShapeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientShapeChanged(EventArgs e)
		{
			if(AfterRubberbandGradientShapeChanged != null)
			{
				AfterRubberbandGradientShapeChanged(this, e);
			}
		}

		#endregion RubberbandGradientShape EventHandlers

		#endregion RubberbandGradientShape Property and EventHandlers



		#region RubberbandGradientShapeFocus Property and EventHandlers

		#region RubberbandGradientShapeFocus Property

		/// <summary>
		/// Focus or center of the gradient that is used for rubberband Gradient.
		/// Default-value: 0.5
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Focus or center of the gradient that is used for rubberband Gradient.\nDefault-value: 0.5\nMin-value: 0.0\nMax-value: 1.0"),
		DefaultValue(0.5f)
		]
		public float RubberbandGradientShapeFocus
		{
			get
			{
				return fRubberbandGradientShapeFocus;
			}
			set
			{
				if(fRubberbandGradientShapeFocus != value &&
					value >= 0.0f &&
					value <= 1.0f)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(fRubberbandGradientShapeFocus, value);
					OnBeforeRubberbandGradientShapeFocusChanged(e);

					if(!e.Cancel)
					{
						fRubberbandGradientShapeFocus = value;

						OnAfterRubberbandGradientShapeFocusChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientShapeFocus Property



		#region RubberbandGradientShapeFocus EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientShapeFocus property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientShapeFocus property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientShapeFocusChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientShapeFocusChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientShapeFocusChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientShapeFocusChanged != null)
			{
				BeforeRubberbandGradientShapeFocusChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientShapeFocus property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientShapeFocus property has changed.")
		]
		public event EventHandler AfterRubberbandGradientShapeFocusChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientShapeFocusChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientShapeFocusChanged(EventArgs e)
		{
			if(AfterRubberbandGradientShapeFocusChanged != null)
			{
				AfterRubberbandGradientShapeFocusChanged(this, e);
			}
		}

		#endregion RubberbandGradientShapeFocus EventHandlers

		#endregion RubberbandGradientShapeFocus Property and EventHandlers



		#region RubberbandGradientShapeScale Property and EventHandlers

		#region RubberbandGradientShapeScale Property

		/// <summary>
		/// Scale or speed of color falloff from focus point that is used for rubberband Gradient.
		/// Default-value: 1.0
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Scale or speed of color falloff from focus point that is used for rubberband Gradient.\nDefault-value: 1.0\nMin-value: 0.0\nMax-value: 1.0"),
		DefaultValue(1.0f)
		]
		public float RubberbandGradientShapeScale
		{
			get
			{
				return fRubberbandGradientShapeScale;
			}
			set
			{
				if(fRubberbandGradientShapeScale != value &&
					value >= 0.0f &&
					value <= 1.0f)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(fRubberbandGradientShapeScale, value);
					OnBeforeRubberbandGradientShapeScaleChanged(e);

					if(!e.Cancel)
					{
						fRubberbandGradientShapeScale = value;

						OnAfterRubberbandGradientShapeScaleChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandGradientShapeScale Property



		#region RubberbandGradientShapeScale EventHandlers

		/// <summary>
		/// Occurs before the RubberbandGradientShapeScale property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs before the RubberbandGradientShapeScale property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandGradientShapeScaleChanged;

		/// <summary>
		/// Raises the BeforeRubberbandGradientShapeScaleChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandGradientShapeScaleChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandGradientShapeScaleChanged != null)
			{
				BeforeRubberbandGradientShapeScaleChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandGradientShapeScale property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Gradient"),
		Description("Occurs after the RubberbandGradientShapeScale property has changed.")
		]
		public event EventHandler AfterRubberbandGradientShapeScaleChanged;

		/// <summary>
		/// Raises the AfterRubberbandGradientShapeScaleChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandGradientShapeScaleChanged(EventArgs e)
		{
			if(AfterRubberbandGradientShapeScaleChanged != null)
			{
				AfterRubberbandGradientShapeScaleChanged(this, e);
			}
		}

		#endregion RubberbandGradientShapeScale EventHandlers

		#endregion RubberbandGradientShapeScale Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Gradient' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Hatch' Properties

		#region RubberbandHatchBackColor Property and EventHandlers

		#region RubberbandHatchBackColor Property

		/// <summary>
		/// BackColor that is used for rubberband Hatch.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("BackColor that is used for rubberband Hatch."),
		DefaultValue(typeof(Color), "IndianRed")
		]
		public Color RubberbandHatchBackColor
		{
			get
			{
				if(cRubberbandHatchBackColor.A != 255 || this.DesignMode)
				{
					return cRubberbandHatchBackColor;
				}
				else
				{
					return Color.FromArgb(iRubberbandHatchBackColorAlpha, cRubberbandHatchBackColor);
				}
			}
			set
			{
				if(cRubberbandHatchBackColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandHatchBackColor, value);
					OnBeforeRubberbandHatchBackColorChanged(e);

					if(!e.Cancel)
					{
						cRubberbandHatchBackColor = value;

						OnAfterRubberbandHatchBackColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandHatchBackColor Property



		#region RubberbandHatchBackColor EventHandlers

		/// <summary>
		/// Occurs before the RubberbandHatchBackColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs before the RubberbandHatchBackColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandHatchBackColorChanged;

		/// <summary>
		/// Raises the BeforeRubberbandHatchBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandHatchBackColorChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandHatchBackColorChanged != null)
			{
				BeforeRubberbandHatchBackColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandHatchBackColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs after the RubberbandHatchBackColor property has changed.")
		]
		public event EventHandler AfterRubberbandHatchBackColorChanged;

		/// <summary>
		/// Raises the AfterRubberbandHatchBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandHatchBackColorChanged(EventArgs e)
		{
			if(AfterRubberbandHatchBackColorChanged != null)
			{
				AfterRubberbandHatchBackColorChanged(this, e);
			}
		}

		#endregion RubberbandHatchBackColor EventHandlers

		#endregion RubberbandHatchBackColor Property and EventHandlers



		#region RubberbandHatchBackColorAlpha Property and EventHandlers

		#region RubberbandHatchBackColorAlpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandHatchBackColor if the RubberbandHatchBackColor has no alpha value set.
		///		I.e. it is used when RubberbandHatchBackColor is 100% opaque.
		///		If RubberbandHatchBackColor is translucent this variable is ignored.
		/// Default-value: 64
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Alpha channel value to be used with the RubberbandHatchBackColor if the RubberbandHatchBackColor has no alpha value set.\nI.e. it is used when RubberbandHatchBackColor is 100% opaque.\nIf RubberbandHatchBackColor is translucent this variable is ignored.\nDefault-value: 64\nMin-value: 0\nMax-value: 255"),
		DefaultValue(64)
		]
		public int RubberbandHatchBackColorAlpha
		{
			get
			{
				return iRubberbandHatchBackColorAlpha;
			}
			set
			{
				if(iRubberbandHatchBackColorAlpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandHatchBackColorAlpha, value);
					OnBeforeRubberbandHatchBackColorAlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandHatchBackColorAlpha = value;

						OnAfterRubberbandHatchBackColorAlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandHatchBackColorAlpha Property



		#region RubberbandHatchBackColorAlpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandHatchBackColorAlpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs before the RubberbandHatchBackColorAlpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandHatchBackColorAlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandHatchBackColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandHatchBackColorAlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandHatchBackColorAlphaChanged != null)
			{
				BeforeRubberbandHatchBackColorAlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandHatchBackColorAlpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs after the RubberbandHatchBackColorAlpha property has changed.")
		]
		public event EventHandler AfterRubberbandHatchBackColorAlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandHatchBackColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandHatchBackColorAlphaChanged(EventArgs e)
		{
			if(AfterRubberbandHatchBackColorAlphaChanged != null)
			{
				AfterRubberbandHatchBackColorAlphaChanged(this, e);
			}
		}

		#endregion RubberbandHatchBackColorAlpha EventHandlers

		#endregion RubberbandHatchBackColorAlpha Property and EventHandlers



		#region RubberbandHatchForeColor Property and EventHandlers

		#region RubberbandHatchForeColor Property

		/// <summary>
		/// ForeColor that is used for rubberband Hatch.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("ForeColor that is used for rubberband Hatch."),
		DefaultValue(typeof(Color), "CornflowerBlue")
		]
		public Color RubberbandHatchForeColor
		{
			get
			{
				if(cRubberbandHatchForeColor.A != 255 || this.DesignMode)
				{
					return cRubberbandHatchForeColor;
				}
				else
				{
					return Color.FromArgb(iRubberbandHatchForeColorAlpha, cRubberbandHatchForeColor);
				}
			}
			set
			{
				if(cRubberbandHatchForeColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cRubberbandHatchForeColor, value);
					OnBeforeRubberbandHatchForeColorChanged(e);

					if(!e.Cancel)
					{
						cRubberbandHatchForeColor = value;

						OnAfterRubberbandHatchForeColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandHatchForeColor Property



		#region RubberbandHatchForeColor EventHandlers

		/// <summary>
		/// Occurs before the RubberbandHatchForeColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs before the RubberbandHatchForeColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandHatchForeColorChanged;

		/// <summary>
		/// Raises the BeforeRubberbandHatchForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandHatchForeColorChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandHatchForeColorChanged != null)
			{
				BeforeRubberbandHatchForeColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandHatchForeColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs after the RubberbandHatchForeColor property has changed.")
		]
		public event EventHandler AfterRubberbandHatchForeColorChanged;

		/// <summary>
		/// Raises the AfterRubberbandHatchForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandHatchForeColorChanged(EventArgs e)
		{
			if(AfterRubberbandHatchForeColorChanged != null)
			{
				AfterRubberbandHatchForeColorChanged(this, e);
			}
		}

		#endregion RubberbandHatchForeColor EventHandlers

		#endregion RubberbandHatchForeColor Property and EventHandlers



		#region RubberbandHatchForeColorAlpha Property and EventHandlers

		#region RubberbandHatchForeColorAlpha Property

		/// <summary>
		/// Alpha channel value to be used with the RubberbandHatchForeColor if the RubberbandHatchForeColor has no alpha value set.
		///		I.e. it is used when RubberbandHatchForeColor is 100% opaque.
		///		If RubberbandHatchForeColor is translucent this variable is ignored.
		/// Default-value: 64
		/// Min-value: 0
		/// Max-value: 255
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Alpha channel value to be used with the RubberbandHatchForeColor if the RubberbandHatchForeColor has no alpha value set.\nI.e. it is used when RubberbandHatchForeColor is 100% opaque.\nIf RubberbandHatchForeColor is translucent this variable is ignored.\nDefault-value: 64\nMin-value: 0\nMax-value: 255"),
		DefaultValue(192)
		]
		public int RubberbandHatchForeColorAlpha
		{
			get
			{
				return iRubberbandHatchForeColorAlpha;
			}
			set
			{
				if(iRubberbandHatchForeColorAlpha != value &&
					value >= 0 &&
					value <= 255)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandHatchForeColorAlpha, value);
					OnBeforeRubberbandHatchForeColorAlphaChanged(e);

					if(!e.Cancel)
					{
						iRubberbandHatchForeColorAlpha = value;

						OnAfterRubberbandHatchForeColorAlphaChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandHatchForeColorAlpha Property



		#region RubberbandHatchForeColorAlpha EventHandlers

		/// <summary>
		/// Occurs before the RubberbandHatchForeColorAlpha property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs before the RubberbandHatchForeColorAlpha property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandHatchForeColorAlphaChanged;

		/// <summary>
		/// Raises the BeforeRubberbandHatchForeColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandHatchForeColorAlphaChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandHatchForeColorAlphaChanged != null)
			{
				BeforeRubberbandHatchForeColorAlphaChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandHatchForeColorAlpha property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs after the RubberbandHatchForeColorAlpha property has changed.")
		]
		public event EventHandler AfterRubberbandHatchForeColorAlphaChanged;

		/// <summary>
		/// Raises the AfterRubberbandHatchForeColorAlphaChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandHatchForeColorAlphaChanged(EventArgs e)
		{
			if(AfterRubberbandHatchForeColorAlphaChanged != null)
			{
				AfterRubberbandHatchForeColorAlphaChanged(this, e);
			}
		}

		#endregion RubberbandHatchForeColorAlpha EventHandlers

		#endregion RubberbandHatchForeColorAlpha Property and EventHandlers



		#region RubberbandHatchStyle Property and EventHandlers

		#region RubberbandHatchStyle Property

		/// <summary>
		/// HatchStyle that is used for rubberband Hatch.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("HatchStyle that is used for rubberband Hatch."),
		DefaultValue(typeof(HatchStyle), "Weave"),
		Editor(typeof(EditorHatchStyle), typeof(UITypeEditor))
		]
		public HatchStyle RubberbandHatchStyle
		{
			get
			{
				return hsRubberbandHatchStyle;
			}
			set
			{
				if(hsRubberbandHatchStyle != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(hsRubberbandHatchStyle, value);
					OnBeforeRubberbandHatchStyleChanged(e);

					if(!e.Cancel)
					{
						hsRubberbandHatchStyle = value;

						OnAfterRubberbandHatchStyleChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandHatchStyle Property



		#region RubberbandHatchStyle EventHandlers

		/// <summary>
		/// Occurs before the RubberbandHatchStyle property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs before the RubberbandHatchStyle property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandHatchStyleChanged;

		/// <summary>
		/// Raises the BeforeRubberbandHatchStyleChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandHatchStyleChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandHatchStyleChanged != null)
			{
				BeforeRubberbandHatchStyleChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandHatchStyle property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Hatch"),
		Description("Occurs after the RubberbandHatchStyle property has changed.")
		]
		public event EventHandler AfterRubberbandHatchStyleChanged;

		/// <summary>
		/// Raises the AfterRubberbandHatchStyleChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandHatchStyleChanged(EventArgs e)
		{
			if(AfterRubberbandHatchStyleChanged != null)
			{
				AfterRubberbandHatchStyleChanged(this, e);
			}
		}

		#endregion RubberbandHatchStyle EventHandlers

		#endregion RubberbandHatchStyle Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Hatch' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Image' Properties

		#region RubberbandImage Property and EventHandlers

		#region RubberbandImage Property

		/// <summary>
		/// Image that is used for rubberband Image.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Image that is used for rubberband Image."),
		DefaultValue(null)
		]
		public Image RubberbandImage
		{
			get
			{
				return imgRubberbandImage;
			}
			set
			{
				if(imgRubberbandImage != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(imgRubberbandImage, value);
					OnBeforeRubberbandImageChanged(e);

					if(!e.Cancel)
					{
						imgRubberbandImage = value;

						OnAfterRubberbandImageChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandImage Property



		#region RubberbandImage EventHandlers

		/// <summary>
		/// Occurs before the RubberbandImage property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs before the RubberbandImage property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandImageChanged;

		/// <summary>
		/// Raises the BeforeRubberbandImageChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandImageChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandImageChanged != null)
			{
				BeforeRubberbandImageChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandImage property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs after the RubberbandImage property has changed.")
		]
		public event EventHandler AfterRubberbandImageChanged;

		/// <summary>
		/// Raises the AfterRubberbandImageChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandImageChanged(EventArgs e)
		{
			if(AfterRubberbandImageChanged != null)
			{
				AfterRubberbandImageChanged(this, e);
			}
		}

		#endregion RubberbandImage EventHandlers

		#endregion RubberbandImage Property and EventHandlers



		#region RubberbandImageScaleCorrection Property and EventHandlers

		#region RubberbandImageScaleCorrection Property

		/// <summary>
		/// True if an image that is used for rubberband Image should be scale-corrected or false otherwise.
		/// Scale-correction is needed when an image is rescaled to more than its original size and a 'whitish line' appears at the bottom and to
		///		the right of the image.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("True if an image that is used for rubberband Image should be scale-corrected or false otherwise.\nScale-correction is needed when an image is rescaled to more than its original size and a 'whitish line' appears at the bottom and to the right of the image.\nNote that when setting this to true one pixel-row is lost at the bottom and one pixel-column is lost at the right of the image."),
		DefaultValue(false)
		]
		public bool RubberbandImageScaleCorrection
		{
			get
			{
				return bRubberbandImageScaleCorrection;
			}
			set
			{
				if(bRubberbandImageScaleCorrection != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bRubberbandImageScaleCorrection, value);
					OnBeforeRubberbandImageScaleCorrectionChanged(e);

					if(!e.Cancel)
					{
						bRubberbandImageScaleCorrection = value;

						OnAfterRubberbandImageScaleCorrectionChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandImageScaleCorrection Property



		#region RubberbandImageScaleCorrection EventHandlers

		/// <summary>
		/// Occurs before the RubberbandImageScaleCorrection property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs before the RubberbandImageScaleCorrection property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandImageScaleCorrectionChanged;

		/// <summary>
		/// Raises the BeforeRubberbandImageScaleCorrectionChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandImageScaleCorrectionChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandImageScaleCorrectionChanged != null)
			{
				BeforeRubberbandImageScaleCorrectionChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandImageScaleCorrection property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs after the RubberbandImageScaleCorrection property has changed.")
		]
		public event EventHandler AfterRubberbandImageScaleCorrectionChanged;

		/// <summary>
		/// Raises the AfterRubberbandImageScaleCorrectionChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandImageScaleCorrectionChanged(EventArgs e)
		{
			if(AfterRubberbandImageScaleCorrectionChanged != null)
			{
				AfterRubberbandImageScaleCorrectionChanged(this, e);
			}
		}

		#endregion RubberbandImageScaleCorrection EventHandlers

		#endregion RubberbandImageScaleCorrection Property and EventHandlers



		#region RubberbandImageTransparency Property and EventHandlers

		#region RubberbandImageTransparency Property

		/// <summary>
		/// The transparency level for the image that is used for rubberband Image.
		/// Default-value: 0.5
		/// Min-value: 0.0
		/// Max-value: 1.0
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("The transparency level for the image that is used for rubberband Image.\nDefault-value: 0.5\nMin-value: 0.0\nMax-value: 1.0"),
		DefaultValue(0.5f)
		]
		public float RubberbandImageTransparency
		{
			get
			{
				return fRubberbandImageTransparency;
			}
			set
			{
				if(	fRubberbandImageTransparency != value &&
					value >= 0.0f && value <= 1.0f)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(fRubberbandImageTransparency, value);
					OnBeforeRubberbandImageTransparencyChanged(e);

					if(!e.Cancel)
					{
						fRubberbandImageTransparency = value;

						OnAfterRubberbandImageTransparencyChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandImageTransparency Property



		#region RubberbandImageTransparency EventHandlers

		/// <summary>
		/// Occurs before the RubberbandImageTransparency property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs before the RubberbandImageTransparency property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandImageTransparencyChanged;

		/// <summary>
		/// Raises the BeforeRubberbandImageTransparencyChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandImageTransparencyChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandImageTransparencyChanged != null)
			{
				BeforeRubberbandImageTransparencyChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandImageTransparency property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Image"),
		Description("Occurs after the RubberbandImageTransparency property has changed.")
		]
		public event EventHandler AfterRubberbandImageTransparencyChanged;

		/// <summary>
		/// Raises the AfterRubberbandImageTransparencyChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandImageTransparencyChanged(EventArgs e)
		{
			if(AfterRubberbandImageTransparencyChanged != null)
			{
				AfterRubberbandImageTransparencyChanged(this, e);
			}
		}

		#endregion RubberbandImageTransparency EventHandlers

		#endregion RubberbandImageTransparency Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Image' Properties



		#region Category 'Behavior, MW - Rubberband-Selection Selection Frame' Properties

		#region RubberbandSelectionFrameActive Property and EventHandlers

		#region RubberbandSelectionFrameActive Property

		/// <summary>
		/// True if the rubberband Selection Frame should be drawn in an active state or false for a passive state.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Selection Frame"),
		Description("True if the rubberband Selection Frame should be drawn in an active state or false for a passive state."),
		DefaultValue(true)
		]
		public bool RubberbandSelectionFrameActive
		{
			get
			{
				return bRubberbandSelectionFrameActive;
			}
			set
			{
				if(bRubberbandSelectionFrameActive != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bRubberbandSelectionFrameActive, value);
					OnBeforeRubberbandSelectionFrameActiveChanged(e);

					if(!e.Cancel)
					{
						bRubberbandSelectionFrameActive = value;

						OnAfterRubberbandSelectionFrameActiveChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandSelectionFrameActive Property



		#region RubberbandSelectionFrameActive EventHandlers

		/// <summary>
		/// Occurs before the RubberbandSelectionFrameActive property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Selection Frame"),
		Description("Occurs before the RubberbandSelectionFrameActive property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandSelectionFrameActiveChanged;

		/// <summary>
		/// Raises the BeforeRubberbandSelectionFrameActiveChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandSelectionFrameActiveChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandSelectionFrameActiveChanged != null)
			{
				BeforeRubberbandSelectionFrameActiveChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandSelectionFrameActive property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband-Selection Selection Frame"),
		Description("Occurs after the RubberbandSelectionFrameActive property has changed.")
		]
		public event EventHandler AfterRubberbandSelectionFrameActiveChanged;

		/// <summary>
		/// Raises the AfterRubberbandSelectionFrameActiveChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandSelectionFrameActiveChanged(EventArgs e)
		{
			if(AfterRubberbandSelectionFrameActiveChanged != null)
			{
				AfterRubberbandSelectionFrameActiveChanged(this, e);
			}
		}

		#endregion RubberbandSelectionFrameActive EventHandlers

		#endregion RubberbandSelectionFrameActive Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband-Selection Selection Frame' Properties



		#region Category 'Behavior, MW - Rubberband Updates' Properties

		#region RubberbandUpdateTimeout Property and EventHandlers

		#region RubberbandUpdateTimeout Property

		/// <summary>
		/// Timeout (ms) between checks to see if an update of the rubberband should be done or not.
		/// Default-value: 200
		/// Min-value: 10
		/// Max-value: 10000
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Timeout (ms) between checks to see if an update of the rubberband should be done or not.\nDefault-value: 200\nMin-value: 10\nMax-value: 10000"),
		DefaultValue(200)
		]
		public int RubberbandUpdateTimeout
		{
			get
			{
				return iRubberbandUpdateTimeout;
			}
			set
			{
				if(	RubberbandUpdateTimeout != value &&
					value >= 10 &&
					value <= 10000)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(iRubberbandUpdateTimeout, value);
					OnBeforeRubberbandUpdateTimeoutChanged(e);

					if(!e.Cancel)
					{
						iRubberbandUpdateTimeout = value;

						tRubberbandUpdate.Interval = iRubberbandUpdateTimeout;

						OnAfterRubberbandUpdateTimeoutChanged(new EventArgs());
					}
				}
			}
		}

		#endregion RubberbandUpdateTimeout Property



		#region RubberbandUpdateTimeout EventHandlers

		/// <summary>
		/// Occurs before the RubberbandUpdateTimeout property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs before the RubberbandUpdateTimeout property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeRubberbandUpdateTimeoutChanged;

		/// <summary>
		/// Raises the BeforeRubberbandUpdateTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeRubberbandUpdateTimeoutChanged(MWCancelEventArgs e)
		{
			if(BeforeRubberbandUpdateTimeoutChanged != null)
			{
				BeforeRubberbandUpdateTimeoutChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the RubberbandUpdateTimeout property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs after the RubberbandUpdateTimeout property has changed.")
		]
		public event EventHandler AfterRubberbandUpdateTimeoutChanged;

		/// <summary>
		/// Raises the AfterRubberbandUpdateTimeoutChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterRubberbandUpdateTimeoutChanged(EventArgs e)
		{
			if(AfterRubberbandUpdateTimeoutChanged != null)
			{
				AfterRubberbandUpdateTimeoutChanged(this, e);
			}
		}

		#endregion RubberbandUpdateTimeout EventHandlers

		#endregion RubberbandUpdateTimeout Property and EventHandlers



		#region UseRubberbandUpdateMouseHover Property and EventHandlers

		#region UseRubberbandUpdateMouseHover Property

		/// <summary>
		/// True if the OnMouseHover EventHandler should check if the rubberband needs updating or false otherwise.
		///	Note that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.
		///	Note that this can be combined with the UseRubberbandUpdateTimer property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("True if the OnMouseHover EventHandler should check if the rubberband needs updating or false otherwise.\nNote that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.\nNote that this can be combined with the UseRubberbandUpdateTimer property."),
		DefaultValue(true)
		]
		public bool UseRubberbandUpdateMouseHover
		{
			get
			{
				return bUseRubberbandUpdateMouseHover;
			}
			set
			{
				if(	UseRubberbandUpdateMouseHover != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bUseRubberbandUpdateMouseHover, value);
					OnBeforeUseRubberbandUpdateMouseHoverChanged(e);

					if(!e.Cancel)
					{
						bUseRubberbandUpdateMouseHover = value;

						OnAfterUseRubberbandUpdateMouseHoverChanged(new EventArgs());
					}
				}
			}
		}

		#endregion UseRubberbandUpdateMouseHover Property



		#region UseRubberbandUpdateMouseHover EventHandlers

		/// <summary>
		/// Occurs before the UseRubberbandUpdateMouseHover property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs before the UseRubberbandUpdateMouseHover property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeUseRubberbandUpdateMouseHoverChanged;

		/// <summary>
		/// Raises the BeforeUseRubberbandUpdateMouseHoverChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeUseRubberbandUpdateMouseHoverChanged(MWCancelEventArgs e)
		{
			if(BeforeUseRubberbandUpdateMouseHoverChanged != null)
			{
				BeforeUseRubberbandUpdateMouseHoverChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the UseRubberbandUpdateMouseHover property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs after the UseRubberbandUpdateMouseHover property has changed.")
		]
		public event EventHandler AfterUseRubberbandUpdateMouseHoverChanged;

		/// <summary>
		/// Raises the AfterUseRubberbandUpdateMouseHoverChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterUseRubberbandUpdateMouseHoverChanged(EventArgs e)
		{
			if(AfterUseRubberbandUpdateMouseHoverChanged != null)
			{
				AfterUseRubberbandUpdateMouseHoverChanged(this, e);
			}
		}

		#endregion UseRubberbandUpdateMouseHover EventHandlers

		#endregion UseRubberbandUpdateMouseHover Property and EventHandlers



		#region UseRubberbandUpdateTimer Property and EventHandlers

		#region UseRubberbandUpdateTimer Property

		/// <summary>
		/// True if the RubberbandUpdateTimer should check if the rubberband needs updating or false otherwise.
		///	Note that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.
		///	Note that this can be combined with the UseRubberbandUpdateMouseHover property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("True if the RubberbandUpdateTimer should check if the rubberband needs updating or false otherwise.\nNote that this should only be used if the MWTreeView is scrolled while the rubberband is being painted.\nNote that this can be combined with the UseRubberbandUpdateMouseHover property."),
		DefaultValue(true)
		]
		public bool UseRubberbandUpdateTimer
		{
			get
			{
				return bUseRubberbandUpdateTimer;
			}
			set
			{
				if(	UseRubberbandUpdateTimer != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bUseRubberbandUpdateTimer, value);
					OnBeforeUseRubberbandUpdateTimerChanged(e);

					if(!e.Cancel)
					{
						bUseRubberbandUpdateTimer = value;

						OnAfterUseRubberbandUpdateTimerChanged(new EventArgs());
					}
				}
			}
		}

		#endregion UseRubberbandUpdateTimer Property



		#region UseRubberbandUpdateTimer EventHandlers

		/// <summary>
		/// Occurs before the UseRubberbandUpdateTimer property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs before the UseRubberbandUpdateTimer property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeUseRubberbandUpdateTimerChanged;

		/// <summary>
		/// Raises the BeforeUseRubberbandUpdateTimerChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeUseRubberbandUpdateTimerChanged(MWCancelEventArgs e)
		{
			if(BeforeUseRubberbandUpdateTimerChanged != null)
			{
				BeforeUseRubberbandUpdateTimerChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the UseRubberbandUpdateTimer property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Rubberband Updates"),
		Description("Occurs after the UseRubberbandUpdateTimer property has changed.")
		]
		public event EventHandler AfterUseRubberbandUpdateTimerChanged;

		/// <summary>
		/// Raises the AfterUseRubberbandUpdateTimerChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterUseRubberbandUpdateTimerChanged(EventArgs e)
		{
			if(AfterUseRubberbandUpdateTimerChanged != null)
			{
				AfterUseRubberbandUpdateTimerChanged(this, e);
			}
		}

		#endregion UseRubberbandUpdateTimer EventHandlers

		#endregion UseRubberbandUpdateTimer Property and EventHandlers

		#endregion Category 'Behavior, MW - Rubberband Updates' Properties



		#region Category 'Behavior, MW - Selection' Properties

		#region AllowNoSelNode Property and EventHandlers

		#region AllowNoSelNode Property

		/// <summary>
		/// True if no TreeNode has to be selected or false otherwise (false is standard for MS TreeView).
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("True if no TreeNode has to be selected or false otherwise (false is standard for MS TreeView)."),
		DefaultValue(true)
		]
		public bool AllowNoSelNode
		{
			get
			{
				return bAllowNoSelNode;
			}
			set
			{
				if(bAllowNoSelNode != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bAllowNoSelNode, value);
					OnBeforeAllowNoSelNodeChanged(e);

					if(!e.Cancel)
					{
						bAllowNoSelNode = value;

						if(!bAllowNoSelNode)
						{
							bActive = false;
							EnsureSelNodeNotNull();
						}

						OnAfterAllowNoSelNodeChanged(new EventArgs());
					}
				}
			}
		}

		#endregion AllowNoSelNode Property



		#region AllowNoSelNode EventHandlers

		/// <summary>
		/// Occurs before the AllowNoSelNode property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the AllowNoSelNode property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeAllowNoSelNodeChanged;

		/// <summary>
		/// Raises the BeforeAllowNoSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeAllowNoSelNodeChanged(MWCancelEventArgs e)
		{
			if(BeforeAllowNoSelNodeChanged != null)
			{
				BeforeAllowNoSelNodeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the AllowNoSelNode property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the AllowNoSelNode property has changed.")
		]
		public event EventHandler AfterAllowNoSelNodeChanged;

		/// <summary>
		/// Raises the AfterAllowNoSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterAllowNoSelNodeChanged(EventArgs e)
		{
			if(AfterAllowNoSelNodeChanged != null)
			{
				AfterAllowNoSelNodeChanged(this, e);
			}
		}

		#endregion AllowNoSelNode EventHandlers

		#endregion AllowNoSelNode Property and EventHandlers



		#region ExtendedTemporaryMarking Property and EventHandlers

		#region ExtendedTemporaryMarking Property

		/// <summary>
		/// True if TreeNodes with non-plain BackColor and/or ForeColor also should be temporary marked or false otherwise (standard TreeView style).
		/// Temporary marking is when a TreeNode (or next to it) is right-clicked. The TreeNode appears selected but as soon as the right mouse button
		///		is released the 'selection' is removed. For a normal TreeView this does not happen for TreeNodes with BackColor and/or ForeColor
		///		other than the standard plain ones.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("True if TreeNodes with non-plain BackColor and/or ForeColor also should be temporary marked or false otherwise (standard TreeView style).\nTemporary marking is when a TreeNode (or next to it) is right-clicked. The TreeNode appears selected but as soon as the right mouse button\nis released the 'selection' is removed. For a normal TreeView this does not happen for TreeNodes with BackColor and/or ForeColor\nother than the standard plain ones."),
		DefaultValue(true)
		]
		public bool ExtendedTemporaryMarking
		{
			get
			{
				return bExtendedTemporaryMarking;
			}
			set
			{
				if(bExtendedTemporaryMarking != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bExtendedTemporaryMarking, value);
					OnBeforeExtendedTemporaryMarkingChanged(e);

					if(!e.Cancel)
					{
						bExtendedTemporaryMarking = value;

						OnAfterExtendedTemporaryMarkingChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ExtendedTemporaryMarking Property



		#region ExtendedTemporaryMarking EventHandlers

		/// <summary>
		/// Occurs before the ExtendedTemporaryMarking property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the ExtendedTemporaryMarking property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeExtendedTemporaryMarkingChanged;

		/// <summary>
		/// Raises the BeforeExtendedTemporaryMarkingChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeExtendedTemporaryMarkingChanged(MWCancelEventArgs e)
		{
			if(BeforeExtendedTemporaryMarkingChanged != null)
			{
				BeforeExtendedTemporaryMarkingChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ExtendedTemporaryMarking property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the ExtendedTemporaryMarking property has changed.")
		]
		public event EventHandler AfterExtendedTemporaryMarkingChanged;

		/// <summary>
		/// Raises the AfterExtendedTemporaryMarkingChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterExtendedTemporaryMarkingChanged(EventArgs e)
		{
			if(AfterExtendedTemporaryMarkingChanged != null)
			{
				AfterExtendedTemporaryMarkingChanged(this, e);
			}
		}

		#endregion ExtendedTemporaryMarking EventHandlers

		#endregion ExtendedTemporaryMarking Property and EventHandlers



		#region MultiSelect Property and EventHandlers

		#region MultiSelect Property

		/// <summary>
		/// Decides the multi select characteristics of an MWTreeView Control.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Decides the multi select characteristics of an MWTreeView Control."),
		DefaultValue(TreeViewMultiSelect.Multi)
		]
		public TreeViewMultiSelect MultiSelect
		{
			get
			{
				return tvmsMultiSelect;
			}
			set
			{
				if(tvmsMultiSelect != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(tvmsMultiSelect, value);
					OnBeforeMultiSelectChanged(e);

					if(!e.Cancel)
					{
						if(tvmsMultiSelect == TreeViewMultiSelect.Classic)
						{
							tvmsMultiSelect = value;

							ClearSelNodes(this.SelectedNode);

							SelectNode(this.SelectedNode, true, false, false);
						}
						else if(value == TreeViewMultiSelect.Classic)
						{
							tvmsMultiSelect = value;

							this.SelectedNode = this.SelNode;

							ClearSelNodes(this.SelNode);
							ClearSelNodes(false);

							this.SelNodeIntSilent = null;
						}
						else if(value == TreeViewMultiSelect.NoMulti)
						{
							tvmsMultiSelect = value;

							SelectNode(this.SelectedNode, true, false, false);

							ClearSelNodes(this.SelectedNode);
						}
						else if(tvmsMultiSelect == TreeViewMultiSelect.NoMulti)
						{
							tvmsMultiSelect = value;

							SelectNode(this.SelectedNode, true);
						}
						else if(value == TreeViewMultiSelect.MultiSameBranchAndLevel)
						{
							tvmsMultiSelect = value;

							TreeNode tnGrandParent = GetTreeNodeGrandParent(this.SelNode);
							int iLevel = GetTreeNodeLevel(this.SelNode);

							Hashtable ht = new Hashtable();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								if(tnGrandParent == GetTreeNodeGrandParent(mwtnw.Node) && iLevel == GetTreeNodeLevel(mwtnw.Node))
								{
									ht.Add(mwtnw.Node.GetHashCode(), mwtnw);
								}
							}

							this.SelNodes = ht;
						}
						else if(value == TreeViewMultiSelect.MultiSameBranch)
						{
							tvmsMultiSelect = value;

							TreeNode tnGrandParent = GetTreeNodeGrandParent(this.SelNode);

							Hashtable ht = new Hashtable();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								if(tnGrandParent == GetTreeNodeGrandParent(mwtnw.Node))
								{
									ht.Add(mwtnw.Node.GetHashCode(), mwtnw);
								}
							}

							this.SelNodes = ht;
						}
						else if(value == TreeViewMultiSelect.MultiSameLevel)
						{
							tvmsMultiSelect = value;

							int iLevel = GetTreeNodeLevel(this.SelNode);

							Hashtable ht = new Hashtable();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								if(iLevel == GetTreeNodeLevel(mwtnw.Node))
								{
									ht.Add(mwtnw.Node.GetHashCode(), mwtnw);
								}
							}

							this.SelNodes = ht;
						}
						else if(value == TreeViewMultiSelect.MultiPathToParents ||
							value == TreeViewMultiSelect.SinglePathToParents)
						{
							tvmsMultiSelect = value;

							ArrayList alTreeNodeGrandParent = new ArrayList();
							ArrayList alTreeNodeLevel = new ArrayList();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								TreeNode tnGrandParent = GetTreeNodeGrandParent(mwtnw.Node);

								int iIndex = alTreeNodeGrandParent.IndexOf(tnGrandParent);

								if(iIndex == -1)
								{
									alTreeNodeGrandParent.Add(tnGrandParent);
									alTreeNodeLevel.Add(GetTreeNodeLevel(mwtnw.Node));
								}
								else
								{
									alTreeNodeLevel[iIndex] = Math.Max((int)alTreeNodeLevel[iIndex], GetTreeNodeLevel(mwtnw.Node));
								}
							}

							Hashtable ht = new Hashtable();

							for(int i = 0; i < alTreeNodeGrandParent.Count; i++)
							{
								foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
								{
									if(	(int)alTreeNodeLevel[i] == GetTreeNodeLevel(mwtnw.Node) &&
										alTreeNodeGrandParent[i] == GetTreeNodeGrandParent(mwtnw.Node) &&
										!ht.Contains(mwtnw.Node.GetHashCode()))
									{
										ht.Add(mwtnw.Node.GetHashCode(), mwtnw);

										if(value == TreeViewMultiSelect.SinglePathToParents)
										{
											break;
										}
									}
								}
							}

							this.SelNodes = ht;

							ArrayList al = new ArrayList();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								al.Add(mwtnw.Node);
							}

							for(int i = 0; i < al.Count; i++)
							{
								SelectNode(al[i] as TreeNode, false);
							}
						}
						else if(value == TreeViewMultiSelect.MultiPathToParent)
						{
							tvmsMultiSelect = value;

							TreeNode tnGrandParent = GetTreeNodeGrandParent(this.SelNode);

							int iSelNodeLevel = GetTreeNodeLevel(this.SelNode);

							Hashtable ht = new Hashtable();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								if(tnGrandParent == GetTreeNodeGrandParent(mwtnw.Node) && iSelNodeLevel == GetTreeNodeLevel(mwtnw.Node))
								{
									ht.Add(mwtnw.Node.GetHashCode(), mwtnw);
								}
							}

							this.SelNodes = ht;

							ArrayList al = new ArrayList();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								al.Add(mwtnw.Node);
							}

							for(int i = 0; i < al.Count; i++)
							{
								SelectNode(al[i] as TreeNode, true);
							}
						}
						else if(value == TreeViewMultiSelect.SinglePathToParent)
						{
							tvmsMultiSelect = value;

							TreeNode tn = this.SelNode;

							ClearSelNodesForce();

							SelectNode(tn, true);
						}
						else if(value == TreeViewMultiSelect.MultiAllDescendants)
						{
							tvmsMultiSelect = value;

							ArrayList al = new ArrayList();

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								al.Add(mwtnw.Node);
							}

							this.SelNodes = new Hashtable();

							for(int i = 0; i < al.Count; i++)
							{
								SelectBranch(al[i] as TreeNode, true, true, true);
							}
						}
						else if(value == TreeViewMultiSelect.Multi)
						{
							tvmsMultiSelect = value;
						}
						else
						{
							//Should never end up here!
							tvmsMultiSelect = value;

							ClearSelNodes();
						}

						if(!IsTreeNodeSelected(this.SelNode))
						{
							this.SelNode = null;

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								SelectNode(mwtnw.Node, true);
								break;
							}
						}

						if(!this.AllowNoSelNode)
						{
							EnsureSelNodeNotNull();
						}

						OnAfterMultiSelectChanged(new EventArgs());
					}
				}
			}
		}

		#endregion MultiSelect Property



		#region MultiSelect EventHandlers

		/// <summary>
		/// Occurs before the MultiSelect property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the MultiSelect property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeMultiSelectChanged;

		/// <summary>
		/// Raises the BeforeMultiSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeMultiSelectChanged(MWCancelEventArgs e)
		{
			if(BeforeMultiSelectChanged != null)
			{
				BeforeMultiSelectChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the MultiSelect property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the MultiSelect property has changed.")
		]
		public event EventHandler AfterMultiSelectChanged;

		/// <summary>
		/// Raises the AfterMultiSelectChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterMultiSelectChanged(EventArgs e)
		{
			if(AfterMultiSelectChanged != null)
			{
				AfterMultiSelectChanged(this, e);
			}
		}

		#endregion MultiSelect EventHandlers

		#endregion MultiSelect Property and EventHandlers



		#region ScrollToSelNode Property and EventHandlers

		#region ScrollToSelNode Property

		/// <summary>
		/// True if scrolling is done so the SelNode (last selected tree node) is always displayed or false otherwise.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("True if scrolling is done so the SelNode (last selected tree node) is always displayed or false otherwise."),
		DefaultValue(true)
		]
		public bool ScrollToSelNode
		{
			get
			{
				return bScrollToSelNode;
			}
			set
			{
				if(bScrollToSelNode != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bScrollToSelNode, value);
					OnBeforeScrollToSelNodeChanged(e);

					if(!e.Cancel)
					{
						if(bScrollToSelNode != value && value)
						{
							if(this.SelNode != null)
							{
								this.SelNode.EnsureVisible();
							}
						}

						bScrollToSelNode = value;

						OnAfterScrollToSelNodeChanged(new EventArgs());
					}
				}
			}
		}

		#endregion ScrollToSelNode Property



		#region ScrollToSelNode EventHandlers

		/// <summary>
		/// Occurs before the ScrollToSelNode property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the ScrollToSelNode property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeScrollToSelNodeChanged;

		/// <summary>
		/// Raises the BeforeScrollToSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeScrollToSelNodeChanged(MWCancelEventArgs e)
		{
			if(BeforeScrollToSelNodeChanged != null)
			{
				BeforeScrollToSelNodeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the ScrollToSelNode property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the ScrollToSelNode property has changed.")
		]
		public event EventHandler AfterScrollToSelNodeChanged;

		/// <summary>
		/// Raises the AfterScrollToSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterScrollToSelNodeChanged(EventArgs e)
		{
			if(AfterScrollToSelNodeChanged != null)
			{
				AfterScrollToSelNodeChanged(this, e);
			}
		}

		#endregion ScrollToSelNode EventHandlers

		#endregion ScrollToSelNode Property and EventHandlers



		#region SelectNodeRegEx Property and EventHandlers

		#region SelectNodeRegEx Property

		/// <summary>
		/// Regular expression that has to be satisfied before a TreeNode can be selected.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Regular expression that has to be satisfied before a TreeNode can be selected."),
		DefaultValue("")
		]
		public string SelectNodeRegEx
		{
			get
			{
				return strSelectNodeRegEx;
			}
			set
			{
				if(strSelectNodeRegEx != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(strSelectNodeRegEx, value);
					OnBeforeSelectNodeRegExChanged(e);

					if(!e.Cancel)
					{
						strSelectNodeRegEx = value;

						EnsureAllSelectedNodesAreAllowed();

						OnAfterSelectNodeRegExChanged(new EventArgs());
					}
				}
			}
		}

		#endregion SelectNodeRegEx Property



		#region SelectNodeRegEx EventHandlers

		/// <summary>
		/// Occurs before the SelectNodeRegEx property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the SelectNodeRegEx property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelectNodeRegExChanged;

		/// <summary>
		/// Raises the BeforeSelectNodeRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelectNodeRegExChanged(MWCancelEventArgs e)
		{
			if(BeforeSelectNodeRegExChanged != null)
			{
				BeforeSelectNodeRegExChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the SelectNodeRegEx property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the SelectNodeRegEx property has changed.")
		]
		public event EventHandler AfterSelectNodeRegExChanged;

		/// <summary>
		/// Raises the AfterSelectNodeRegExChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelectNodeRegExChanged(EventArgs e)
		{
			if(AfterSelectNodeRegExChanged != null)
			{
				AfterSelectNodeRegExChanged(this, e);
			}
		}

		#endregion SelectNodeRegEx EventHandlers

		#endregion SelectNodeRegEx Property and EventHandlers



		#region SelectedTreeNodeBackColor Property and EventHandlers

		#region SelectedTreeNodeBackColor Property

		/// <summary>
		/// The BackColor of a selected TreeNode.
		/// Note that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("The BackColor of a selected TreeNode.\nNote that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true."),
		DefaultValue(typeof(Color), "Highlight")
		]
		public Color SelectedTreeNodeBackColor
		{
			get
			{
				return cSelectedTreeNodeBackColor;
			}
			set
			{
				if(cSelectedTreeNodeBackColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cSelectedTreeNodeBackColor, value);
					OnBeforeSelectedTreeNodeBackColorChanged(e);

					if(!e.Cancel)
					{
						cSelectedTreeNodeBackColor = value;

						ChangeSelNodesColors();

						OnAfterSelectedTreeNodeBackColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion SelectedTreeNodeBackColor Property



		#region SelectedTreeNodeBackColor EventHandlers

		/// <summary>
		/// Occurs before the SelectedTreeNodeBackColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs before the SelectedTreeNodeBackColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelectedTreeNodeBackColorChanged;

		/// <summary>
		/// Raises the BeforeSelectedTreeNodeBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelectedTreeNodeBackColorChanged(MWCancelEventArgs e)
		{
			if(BeforeSelectedTreeNodeBackColorChanged != null)
			{
				BeforeSelectedTreeNodeBackColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the SelectedTreeNodeBackColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs after the SelectedTreeNodeBackColor property has changed.")
		]
		public event EventHandler AfterSelectedTreeNodeBackColorChanged;

		/// <summary>
		/// Raises the AfterSelectedTreeNodeBackColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelectedTreeNodeBackColorChanged(EventArgs e)
		{
			if(AfterSelectedTreeNodeBackColorChanged != null)
			{
				AfterSelectedTreeNodeBackColorChanged(this, e);
			}
		}

		#endregion SelectedTreeNodeBackColor EventHandlers

		#endregion SelectedTreeNodeBackColor Property and EventHandlers



		#region SelectedTreeNodeForeColor Property and EventHandlers

		#region SelectedTreeNodeForeColor Property

		/// <summary>
		/// The ForeColor of a selected TreeNode.
		/// Note that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("The ForeColor of a selected TreeNode.\nNote that this property is used when a TreeNode is selected and the UseExtendedSelectionColors property is set to true."),
		DefaultValue(typeof(Color), "HighlightText")
		]
		public Color SelectedTreeNodeForeColor
		{
			get
			{
				return cSelectedTreeNodeForeColor;
			}
			set
			{
				if(cSelectedTreeNodeForeColor != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(cSelectedTreeNodeForeColor, value);
					OnBeforeSelectedTreeNodeForeColorChanged(e);

					if(!e.Cancel)
					{
						cSelectedTreeNodeForeColor = value;

						ChangeSelNodesColors();

						OnAfterSelectedTreeNodeForeColorChanged(new EventArgs());
					}
				}
			}
		}

		#endregion SelectedTreeNodeForeColor Property



		#region SelectedTreeNodeForeColor EventHandlers

		/// <summary>
		/// Occurs before the SelectedTreeNodeForeColor property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs before the SelectedTreeNodeForeColor property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelectedTreeNodeForeColorChanged;

		/// <summary>
		/// Raises the BeforeSelectedTreeNodeForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelectedTreeNodeForeColorChanged(MWCancelEventArgs e)
		{
			if(BeforeSelectedTreeNodeForeColorChanged != null)
			{
				BeforeSelectedTreeNodeForeColorChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the SelectedTreeNodeForeColor property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs after the SelectedTreeNodeForeColor property has changed.")
		]
		public event EventHandler AfterSelectedTreeNodeForeColorChanged;

		/// <summary>
		/// Raises the AfterSelectedTreeNodeForeColorChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelectedTreeNodeForeColorChanged(EventArgs e)
		{
			if(AfterSelectedTreeNodeForeColorChanged != null)
			{
				AfterSelectedTreeNodeForeColorChanged(this, e);
			}
		}

		#endregion SelectedTreeNodeForeColor EventHandlers

		#endregion SelectedTreeNodeForeColor Property and EventHandlers



		#region UseExtendedSelectionColors Property and EventHandlers

		#region UseExtendedSelectionColors Property

		/// <summary>
		/// True if extended selection colors should be used for TreeNodes or false if the regular selection colors should be used.
		/// Regular BackColor is SystemColors.Highlight and regular ForeColor is SystemColors.HighlightText.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("True if extended selection colors should be used for TreeNodes or false if the regular selection colors should be used.\nRegular BackColor is SystemColors.Highlight and regular ForeColor is SystemColors.HighlightText."),
		DefaultValue(true)
		]
		public bool UseExtendedSelectionColors
		{
			get
			{
				return bUseExtendedSelectionColors;
			}
			set
			{
				if(bUseExtendedSelectionColors != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(bUseExtendedSelectionColors, value);
					OnBeforeUseExtendedSelectionColorsChanged(e);

					if(!e.Cancel)
					{
						bUseExtendedSelectionColors = value;

						ChangeSelNodesColors();

						OnAfterUseExtendedSelectionColorsChanged(new EventArgs());
					}
				}
			}
		}

		#endregion UseExtendedSelectionColors Property



		#region UseExtendedSelectionColors EventHandlers

		/// <summary>
		/// Occurs before the UseExtendedSelectionColors property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs before the UseExtendedSelectionColors property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeUseExtendedSelectionColorsChanged;

		/// <summary>
		/// Raises the BeforeUseExtendedSelectionColorsChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeUseExtendedSelectionColorsChanged(MWCancelEventArgs e)
		{
			if(BeforeUseExtendedSelectionColorsChanged != null)
			{
				BeforeUseExtendedSelectionColorsChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the UseExtendedSelectionColors property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Appearance, MW - Selection"),
		Description("Occurs after the UseExtendedSelectionColors property has changed.")
		]
		public event EventHandler AfterUseExtendedSelectionColorsChanged;

		/// <summary>
		/// Raises the AfterUseExtendedSelectionColorsChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterUseExtendedSelectionColorsChanged(EventArgs e)
		{
			if(AfterUseExtendedSelectionColorsChanged != null)
			{
				AfterUseExtendedSelectionColorsChanged(this, e);
			}
		}

		#endregion UseExtendedSelectionColors EventHandlers

		#endregion UseExtendedSelectionColors Property and EventHandlers

		#endregion Category 'Behavior, MW - Selection' Properties

		#endregion Browsable Properties



		#region Non-Browsable Properties

		#region Category 'Behavior, MW - Checking' Properties

		#region CheckedNodes Property and EventHandlers

		#region CheckedNodes Property

		/// <summary>
		/// HashTable containing the Checked TreeNodes as values and the TreeNode.GetHashCodes as keys.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Checking"),
		Description("HashTable containing the Checked TreeNodes as values and the TreeNode.GetHashCodes as keys."),
		DefaultValue(null)
		]
		public Hashtable CheckedNodes
		{
			get
			{
				return htCheckedNodes;
			}
			set
			{
				if(htCheckedNodes != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(htCheckedNodes, value);
					OnBeforeCheckedNodesChanged(e);

					if(!e.Cancel)
					{
						if(value == null)
						{
							if(this.CheckedNodes == null)
							{
								htCheckedNodes = new Hashtable();
							}

							foreach(TreeNode tn in this.CheckedNodes.Values)
							{
								bForceCheckNode = true;
								tn.Checked = false;
								bForceCheckNode = false;
							}

							htCheckedNodes.Clear();
						}
						else
						{
							if(this.CheckedNodes == null)
							{
								htCheckedNodes = new Hashtable();
							}

							foreach(TreeNode tn in this.CheckedNodes.Values)
							{
								bForceCheckNode = true;
								tn.Checked = false;
								bForceCheckNode = false;
							}

							foreach(TreeNode tn in value.Values)
							{
								bForceCheckNode = true;
								tn.Checked = true;
								bForceCheckNode = false;
							}
						}

						htCheckedNodes = value;

						EnsureAllCheckedNodesAreAllowed();

						OnAfterCheckedNodesChanged(new EventArgs());
					}
				}
			}
		}

		#endregion CheckedNodes Property



		#region CheckedNodes EventHandlers

		/// <summary>
		/// Occurs before the CheckedNodes property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs before the CheckedNodes property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCheckedNodesChanged;

		/// <summary>
		/// Raises the BeforeCheckedNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCheckedNodesChanged(MWCancelEventArgs e)
		{
			if(BeforeCheckedNodesChanged != null)
			{
				BeforeCheckedNodesChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the CheckedNodes property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs after the CheckedNodes property has changed.")
		]
		public event EventHandler AfterCheckedNodesChanged;

		/// <summary>
		/// Raises the AfterCheckedNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCheckedNodesChanged(EventArgs e)
		{
			if(AfterCheckedNodesChanged != null)
			{
				AfterCheckedNodesChanged(this, e);
			}
		}

		#endregion CheckedNodes EventHandlers

		#endregion CheckedNodes Property and EventHandlers

		#endregion Category 'Behavior, MW - Checking' Properties



		#region Category 'Behavior, MW - Selection' Properties

		#region SelNode, SelNodeInt and SelNodeIntSilent Properties, SetSelNodeInt Method and EventHandlers

		#region SelNode Property

		/// <summary>
		/// Last Selected TreeNode or null if no TreeNode is selected or if Last Selected TreeNode was deselected.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("Last Selected TreeNode or null if no TreeNode is selected or if Last Selected TreeNode was deselected."),
		DefaultValue(null)
		]
		public TreeNode SelNode
		{
			get
			{
				return tnSelNode;
			}
			set
			{
				if(value != null)
				{
					SelectNode(value, true); //Changed from tnSelNode to value (stupid mistake really). Thanks to Jason W Farrell for finding the bug.
				}
				else
				{
					DeselectNode(value, true); //Changed from tnSelNode to value (stupid mistake really). Thanks to Jason W Farrell for finding the bug.
				}
			}
		}

		#endregion SelNode Property



		#region SelNodeInt Property

		/// <summary>
		/// Selected TreeNode or null if no TreeNode is selected.
		/// Note that this is an internal property only. EventHandlers are attached to this property though because in order to change the
		///		tnSelNode variable this property has to do it.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("Last Selected TreeNode or null if no TreeNode is selected or if Last Selected TreeNode was deselected.\nFor internal use only."),
		DefaultValue(null)
		]
		protected TreeNode SelNodeInt
		{
			get
			{
				return tnSelNode;
			}
			set
			{
				SetSelNodeInt(value, true);
			}
		}

		#endregion SelNodeInt Property



		#region SelNodeIntSilent Property

		/// <summary>
		/// Selected TreeNode or null if no TreeNode is selected.
		/// Note that this is an internal property only.
		/// No EventHandlers are attached to this property. See SelNodeInt for a property with EventHandlers.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("Last Selected TreeNode or null if no TreeNode is selected or if Last Selected TreeNode was deselected.\nFor internal use only."),
		DefaultValue(null)
		]
		private TreeNode SelNodeIntSilent
		{
			get
			{
				return tnSelNode;
			}
			set
			{
				SetSelNodeInt(value, false);
			}
		}

		#endregion SelNodeIntSilent Property



		#region SetSelNodeInt Method

		/// <summary>
		/// Set the selected TreeNode.
		/// Note that this is an internal method only to be used from the set properties of the SelNodeInt and SelNodeIntSilent properties.
		/// </summary>
		/// <param name="value">TreeNode to set SelNode to.</param>
		/// <param name="bTriggerSelNodeEvents">True if calling this method should raise the OnBeforeSelNodeChanged/OnAfterSelNodeChanged events or false otherwise.</param>
		private void SetSelNodeInt(TreeNode value, bool bTriggerSelNodeEvents)
		{
			if(tnSelNode != value)
			{
				MWCancelEventArgs e = new MWCancelEventArgs(tnSelNode, value);

				if(bTriggerSelNodeEvents)
				{
					OnBeforeSelNodeChanged(e);
				}

				if(!e.Cancel)
				{
					if(value == null || IsSelectNodeRegExSatisfied(value.Text))
					{
						tnSelNode = value;

						if(	(Control.MouseButtons & MouseButtons.Left) == 0 &&
							this.MultiSelect != TreeViewMultiSelect.Classic &&
							this.Enabled)
						{
							bPaintFocusRectAndHottracking = true;
							this.SelectedNode = value;
							bPaintFocusRectAndHottracking = false;
						}
					}

					if(bTriggerSelNodeEvents)
					{
						OnAfterSelNodeChanged(new MWControlSuite.MWPropertyEventArgs(tnSelNode));
					}
				}
			}
			else if(value == null || IsSelectNodeRegExSatisfied(value.Text))
			{
				if(	(Control.MouseButtons & MouseButtons.Left) == 0 &&
					this.MultiSelect != TreeViewMultiSelect.Classic)
				{
					bPaintFocusRectAndHottracking = true;
					this.SelectedNode = value;
					bPaintFocusRectAndHottracking = false;
				}
			}
		}

		#endregion SetSelNodeInt Method



		#region SelNode EventHandlers (raised through property SelNodeInt really (in method SetSelNodeInt))

		/// <summary>
		/// Occurs before the SelNode property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the SelNode property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelNodeChanged;

		/// <summary>
		/// Raises the BeforeSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelNodeChanged(MWCancelEventArgs e)
		{
			if(BeforeSelNodeChanged != null)
			{
				BeforeSelNodeChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the SelNode property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the SelNode property has changed.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterSelNodeChanged;

		/// <summary>
		/// Raises the AfterSelNodeChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelNodeChanged(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterSelNodeChanged != null)
			{
				AfterSelNodeChanged(this, e);
			}
		}

		#endregion SelNode EventHandlers (raised through property SelNodeInt really (in method SetSelNodeInt))

		#endregion SelNode, SelNodeInt and SelNodeIntSilent Properties, SetSelNodeInt Method and EventHandlers



		#region SelNodes Property and EventHandlers

		#region SelNodes Property

		/// <summary>
		/// HashTable containing the Selected TreeNodes wrapped in MWTreeNodeWrapper objects as values and the TreeNode.GetHashCodes as keys.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("HashTable containing the Selected TreeNodes wrapped in MWTreeNodeWrapper objects as values and the TreeNode.GetHashCodes as keys."),
		DefaultValue(null)
		]
		public Hashtable SelNodes
		{
			get
			{
				return htSelNodes;
			}
			set
			{
				if(htSelNodes != value)
				{
					MWCancelEventArgs e = new MWCancelEventArgs(htSelNodes, value);
					OnBeforeSelNodesChanged(e);

					if(!e.Cancel)
					{
						if(value == null)
						{
							this.SelNode = null;

							if(this.SelNodes == null)
							{
								htSelNodes = new Hashtable();
							}

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								MWTreeNodeWrapper.Deselect(mwtnw);
							}

							htSelNodes.Clear();
						}
						else
						{
							if(this.SelNodes == null)
							{
								htSelNodes = new Hashtable();
							}

							foreach(MWTreeNodeWrapper mwtnw in htSelNodes.Values)
							{
								if(!value.Contains(mwtnw.Node.GetHashCode()))
								{
									MWTreeNodeWrapper.Deselect(mwtnw);
								}
							}
						}

						htSelNodes = new Hashtable();

						if(value != null)
						{
							foreach(MWTreeNodeWrapper mwtnw in value.Values)
							{
								MWTreeNodeWrapper.Deselect(mwtnw);
								htSelNodes.Add(mwtnw.Node.GetHashCode(), new MWTreeNodeWrapper(mwtnw.Node, this.UseExtendedSelectionColors, this.SelectedTreeNodeBackColor, this.SelectedTreeNodeForeColor));
							}
						}

						if(this.SelNode != null && !value.Contains(this.SelNode.GetHashCode()))
						{
							this.SelNode = null;
						}

						ActivateOrDeactivateSelNodes();

						EnsureAllSelectedNodesAreAllowed();

						OnAfterSelNodesChanged(new EventArgs());
					}
				}
			}
		}

		#endregion SelNodes Property



		#region SelNodes EventHandlers

		/// <summary>
		/// Occurs before the SelNodes property changes.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before the SelNodes property changes.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelNodesChanged;

		/// <summary>
		/// Raises the BeforeSelNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelNodesChanged(MWCancelEventArgs e)
		{
			if(BeforeSelNodesChanged != null)
			{
				BeforeSelNodesChanged(this, e);
			}
		}

		/// <summary>
		/// Occurs after the SelNodes property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the SelNodes property has changed.")
		]
		public event EventHandler AfterSelNodesChanged;

		/// <summary>
		/// Raises the AfterSelNodesChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelNodesChanged(EventArgs e)
		{
			if(AfterSelNodesChanged != null)
			{
				AfterSelNodesChanged(this, e);
			}
		}

		#endregion SelNodes EventHandlers

		#endregion SelNodes Property and EventHandlers



		#region TreeNodeMouseDown, TreeNodeMouseDownInt Properties and EventHandlers

		#region TreeNodeMouseDown Property

		/// <summary>
		/// TreeNode that was in the coordinates of the OnMouseDown.
		/// Used in conjunction with rubberband selection.
		/// Note that this TreeNode is treated differently than the tnFullRowSelect TreeNode and they must therefore be separate variables.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("TreeNode that the mouse was last clicked on."),
		DefaultValue(null)
		]
		public TreeNode TreeNodeMouseDown
		{
			get
			{
				return tnMouseDown;
			}
		}

		#endregion TreeNodeMouseDown Property



		#region TreeNodeMouseDownInt Property

		/// <summary>
		/// TreeNode that was in the coordinates of the OnMouseDown.
		/// Used in conjunction with rubberband selection.
		/// Note that this TreeNode is treated differently than the tnFullRowSelect TreeNode and they must therefore be separate variables.
		/// Note that this is an internal method, therefore it is private. Use the TreeNodeMouseDown get property to access it from outside this
		///		class.
		/// </summary>
		[
		Browsable(false),
		Category("Behavior, MW - Selection"),
		Description("TreeNode that the mouse was last clicked on."),
		DefaultValue(null)
		]
		private TreeNode TreeNodeMouseDownInt
		{
			get
			{
				return tnMouseDown;
			}
			set
			{
				if(tnMouseDown != value)
				{
					tnMouseDown = value;

					OnAfterTreeNodeMouseDownChanged(new MWControlSuite.MWPropertyEventArgs(tnMouseDown));
				}
			}
		}

		#endregion TreeNodeMouseDownInt Property



		#region TreeNodeMouseDown EventHandlers

		/// <summary>
		/// Occurs after the TreeNodeMouseDown property has changed.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after the TreeNodeMouseDown property has changed.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterTreeNodeMouseDownChanged;

		/// <summary>
		/// Raises the AfterTreeNodeMouseDownChanged Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterTreeNodeMouseDownChanged(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterTreeNodeMouseDownChanged != null)
			{
				AfterTreeNodeMouseDownChanged(this, e);
			}
		}

		#endregion TreeNodeMouseDown EventHandlers

		#endregion TreeNodeMouseDown, TreeNodeMouseDownInt Properties and EventHandlers

		#endregion Category 'Behavior, MW - Selection' Properties

		#endregion Non-Browsable Properties

		#endregion Properties with EventHandlers



		#region Protected Main Methods with EventHandlers

		#region CheckedNodesAdd and CheckedNodesRemove Methods with EventHandlers

		#region CheckedNodesAdd Methods and EventHandlers

		#region CheckedNodesAdd Methods

		/// <summary>
		/// Add a TreeNode to the CheckedNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to add to the CheckedNodes property.</param>
		/// <returns>True if the TreeNode is actually added or false otherwise.</returns>
		protected bool CheckedNodesAdd(TreeNode tn)
		{
			return CheckedNodesAdd(tn, true);
		}

		/// <summary>
		/// Add a TreeNode to the CheckedNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to add to the CheckedNodes property.</param>
		/// <param name="bTriggerEvents">True if calling this method should raise the OnBeforeCheckedNodesAdd and OnAfterCheckedNodesAdd events or false otherwise.</param>
		/// <returns>True if the TreeNode is actually added or false otherwise.</returns>
		private bool CheckedNodesAdd(TreeNode tn, bool bTriggerEvents)
		{
			bool bRetVal = false;

			if(!this.IsTreeNodeChecked(tn))
			{
				MWCancelEventArgs e = new MWCancelEventArgs(this.CheckedNodes, tn);

				if(bTriggerEvents)
				{
					OnBeforeCheckedNodesAdd(e);
				}

				if(!e.Cancel)
				{
					this.CheckedNodes.Add(tn.GetHashCode(), tn);

					if(bTriggerEvents)
					{
						OnAfterCheckedNodesAdd(new MWControlSuite.MWPropertyEventArgs(tn));
					}

					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion CheckedNodesAdd Methods



		#region CheckedNodesAdd EventHandlers

		/// <summary>
		/// Occurs before a TreeNode is added to the CheckedNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs before a TreeNode is added to the CheckedNodes property.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCheckedNodesAdd;

		/// <summary>
		/// Raises the BeforeCheckedNodesAdd Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCheckedNodesAdd(MWCancelEventArgs e)
		{
			if(BeforeCheckedNodesAdd != null)
			{
				BeforeCheckedNodesAdd(this, e);
			}
		}

		/// <summary>
		/// Occurs after a TreeNode is added to the CheckedNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs after a TreeNode is added to the CheckedNodes property.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterCheckedNodesAdd;

		/// <summary>
		/// Raises the AfterCheckedNodesAdd Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCheckedNodesAdd(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterCheckedNodesAdd != null)
			{
				AfterCheckedNodesAdd(this, e);
			}
		}

		#endregion CheckedNodesAdd EventHandlers

		#endregion CheckedNodesAdd Methods and EventHandlers



		#region CheckedNodesRemove Methods and EventHandlers

		#region CheckedNodesRemove Methods

		/// <summary>
		/// Remove a TreeNode to the CheckedNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to remove from the CheckedNodes property.</param>
		/// <returns>True if the TreeNode is actually removed or false otherwise.</returns>
		protected bool CheckedNodesRemove(TreeNode tn)
		{
			return CheckedNodesRemove(tn, true);
		}

		/// <summary>
		/// Remove a TreeNode to the CheckedNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to remove from the CheckedNodes property.</param>
		/// <param name="bTriggerEvents">True if calling this method should raise the OnBeforeCheckedNodesAdd and OnAfterCheckedNodesAdd events or false otherwise.</param>
		/// <returns>True if the TreeNode is actually removed or false otherwise.</returns>
		private bool CheckedNodesRemove(TreeNode tn, bool bTriggerEvents)
		{
			bool bRetVal = false;

			if(this.IsTreeNodeChecked(tn))
			{
				MWCancelEventArgs e = new MWCancelEventArgs(this.CheckedNodes, tn);

				if(bTriggerEvents)
				{
					OnBeforeCheckedNodesRemove(e);
				}

				if(!e.Cancel)
				{
					this.CheckedNodes.Remove(tn.GetHashCode());

					if(bTriggerEvents)
					{
						OnAfterCheckedNodesRemove(new MWControlSuite.MWPropertyEventArgs(tn));
					}

					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion CheckedNodesRemove Methods



		#region CheckedNodesRemove EventHandlers

		/// <summary>
		/// Occurs before a TreeNode is removed from the CheckedNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs before a TreeNode is removed from the CheckedNodes property.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeCheckedNodesRemove;

		/// <summary>
		/// Raises the BeforeCheckedNodesRemove Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeCheckedNodesRemove(MWCancelEventArgs e)
		{
			if(BeforeCheckedNodesRemove != null)
			{
				BeforeCheckedNodesRemove(this, e);
			}
		}

		/// <summary>
		/// Occurs after a TreeNode is removed from the CheckedNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Checking"),
		Description("Occurs after a TreeNode is removed from the CheckedNodes property.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterCheckedNodesRemove;

		/// <summary>
		/// Raises the AfterCheckedNodesRemove Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterCheckedNodesRemove(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterCheckedNodesRemove != null)
			{
				AfterCheckedNodesRemove(this, e);
			}
		}

		#endregion CheckedNodesRemove EventHandlers

		#endregion CheckedNodesRemove Methods and EventHandlers

		#endregion CheckedNodesAdd and CheckedNodesRemove Methods with EventHandlers



		#region SelNodesAdd and SelNodesRemove Methods with EventHandlers

		#region SelNodesAdd Methods and EventHandlers

		#region SelNodesAdd Methods

		/// <summary>
		/// Add a TreeNode to the SelNodes property.
		/// Calling this method raises the OnBeforeSelNodesAdd and OnAfterSelNodesAdd events.
		/// </summary>
		/// <param name="tn">TreeNode to add to the SelNodes property.</param>
		/// <returns>True if the TreeNode is actually added or false otherwise.</returns>
		protected bool SelNodesAdd(TreeNode tn)
		{
			return SelNodesAdd(tn, true);
		}

		/// <summary>
		/// Add a TreeNode to the SelNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to add to the SelNodes property.</param>
		/// <param name="bTriggerEvents">True if calling this method should raise the OnBeforeSelNodesAdd and OnAfterSelNodesAdd events or false otherwise.</param>
		/// <returns>True if the TreeNode is actually added or false otherwise.</returns>
		private bool SelNodesAdd(TreeNode tn, bool bTriggerEvents)
		{
			bool bRetVal = false;

			if(!this.IsTreeNodeSelected(tn))
			{
				MWCancelEventArgs e = new MWCancelEventArgs(this.SelNodes, tn);

				if(bTriggerEvents)
				{
					OnBeforeSelNodesAdd(e);
				}

				if(!e.Cancel)
				{
					this.SelNodes.Add(tn.GetHashCode(), new MWTreeNodeWrapper(tn, this.UseExtendedSelectionColors, this.SelectedTreeNodeBackColor, this.SelectedTreeNodeForeColor));

					if(!bActive)
					{
						DeactivateSelNode(tn);
					}

					if(bTriggerEvents)
					{
						OnAfterSelNodesAdd(new MWControlSuite.MWPropertyEventArgs(tn));
					}

					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion SelNodesAdd Methods



		#region SelNodesAdd EventHandlers

		/// <summary>
		/// Occurs before a TreeNode is added to the SelNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before a TreeNode is added to the SelNodes property.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelNodesAdd;

		/// <summary>
		/// Raises the BeforeSelNodesAdd Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelNodesAdd(MWCancelEventArgs e)
		{
			if(BeforeSelNodesAdd != null)
			{
				BeforeSelNodesAdd(this, e);
			}
		}

		/// <summary>
		/// Occurs after a TreeNode is added to the SelNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after a TreeNode is added to the SelNodes property.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterSelNodesAdd;

		/// <summary>
		/// Raises the AfterSelNodesAdd Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelNodesAdd(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterSelNodesAdd != null)
			{
				AfterSelNodesAdd(this, e);
			}
		}

		#endregion SelNodesAdd EventHandlers

		#endregion SelNodesAdd Methods and EventHandlers



		#region SelNodesRemove Methods and EventHandlers

		#region SelNodesRemove Methods

		/// <summary>
		/// Remove a TreeNode to the SelNodes property.
		/// Calling this method raises the OnBeforeSelNodesRemove and OnAfterSelNodesRemove events.
		/// </summary>
		/// <param name="tn">TreeNode to remove from the SelNodes property.</param>
		/// <returns>True if the TreeNode is actually removed or false otherwise.</returns>
		protected bool SelNodesRemove(TreeNode tn)
		{
			return SelNodesRemove(tn, true);
		}

		/// <summary>
		/// Remove a TreeNode to the SelNodes property.
		/// </summary>
		/// <param name="tn">TreeNode to remove from the SelNodes property.</param>
		/// <param name="bTriggerEvents">True if calling this method should raise the OnBeforeSelNodesRemove and OnAfterSelNodesRemove events or false otherwise.</param>
		/// <returns>True if the TreeNode is actually removed or false otherwise.</returns>
		private bool SelNodesRemove(TreeNode tn, bool bTriggerEvents)
		{
			bool bRetVal = false;

			if(this.IsTreeNodeSelected(tn))
			{
				MWCancelEventArgs e = new MWCancelEventArgs(this.SelNodes, tn);

				if(bTriggerEvents)
				{
					OnBeforeSelNodesRemove(e);
				}

				if(!e.Cancel)
				{
					MWTreeNodeWrapper.Deselect(this.SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper);

					this.SelNodes.Remove(tn.GetHashCode());

					if(bTriggerEvents)
					{
						OnAfterSelNodesRemove(new MWControlSuite.MWPropertyEventArgs(tn));
					}

					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion SelNodesRemove Methods



		#region SelNodesRemove EventHandlers

		/// <summary>
		/// Occurs before a TreeNode is removed from the SelNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs before a TreeNode is removed from the SelNodes property.")
		]
		public event MWControlSuite.MWCancelEventHandler BeforeSelNodesRemove;

		/// <summary>
		/// Raises the BeforeSelNodesRemove Event.
		/// </summary>
		/// <param name="e">Standard MWCancelEventArgs object.</param>
		public void OnBeforeSelNodesRemove(MWCancelEventArgs e)
		{
			if(BeforeSelNodesRemove != null)
			{
				BeforeSelNodesRemove(this, e);
			}
		}

		/// <summary>
		/// Occurs after a TreeNode is removed from the SelNodes property.
		/// </summary>
		[
		Browsable(true),
		Category("Behavior, MW - Selection"),
		Description("Occurs after a TreeNode is removed from the SelNodes property.")
		]
		public event MWControlSuite.MWPropertyEventHandler AfterSelNodesRemove;

		/// <summary>
		/// Raises the AfterSelNodesRemove Event.
		/// </summary>
		/// <param name="e">Standard EventArgs object.</param>
		public void OnAfterSelNodesRemove(MWControlSuite.MWPropertyEventArgs e)
		{
			if(AfterSelNodesRemove != null)
			{
				AfterSelNodesRemove(this, e);
			}
		}

		#endregion SelNodesRemove EventHandlers

		#endregion SelNodesRemove Methods and EventHandlers

		#endregion SelNodesAdd and SelNodesRemove Methods with EventHandlers

		#endregion Protected Main Methods with EventHandlers



		#region Public Main Methods

		#region AddNode and AddNodeInt

		#region AddNode

		/// <summary>
		/// Add a TreeNode by specifying the Text and path to it (from the root) as opposed to adding a TreeNode to an existing TreeNode (or root).
		/// The need for a method like this was expressed by Jonatan Rullman.
		/// </summary>
		/// <param name="strText">Text of the TreeNode.</param>
		/// <param name="bAllowDuplicate">True to create a new TreeNode if there is already a TreeNode with this Text or false otherwise.</param>
		/// <param name="bCreatePath">False if the TreeNode should only be created if the full path described in astrPath exists or true to create the path as necessary.</param>
		/// <param name="astrPath">Path pointing to where the new TreeNode should be created. Each member of the array is a new level starting from the root.</param>
		/// <returns>True if the TreeNode was successfully added or false otherwise.</returns>
		public bool AddNode(string strText, bool bAllowDuplicate, bool bCreatePath, params string[] astrPath)
		{
			bool bRetVal = false;

			TreeNode tnAdd = null;

			TreeNodeCollection tnc = this.Nodes;

			if(astrPath.Length > 0)
			{
				bool bFound = false;

				foreach(string strPath in astrPath)
				{
					bFound = false;

					foreach(TreeNode tn in tnc)
					{
						if(tn.Text == strPath)
						{
							tnc = tn.Nodes;
							bFound = true;
							break;
						}
					}

					if(!bFound)
					{
						if(bCreatePath)
						{
							if(AddNodeInt(tnc, strPath, bAllowDuplicate, out tnAdd))
							{
								tnc = tnAdd.Nodes;

								bFound = true;
							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
				}

				if(bFound)
				{
					bRetVal = AddNodeInt(tnc, strText, bAllowDuplicate, out tnAdd);
				}
			}
			else
			{
				bRetVal = AddNodeInt(this.Nodes, strText, bAllowDuplicate, out tnAdd);
			}

			return bRetVal;
		}

		#endregion AddNode



		#region AddNodeInt

		/// <summary>
		/// Add a TreeNode.
		/// </summary>
		/// <param name="tnc">TreeNodeCollection that the TreeNode should be added to.</param>
		/// <param name="strText">Text of the TreeNode.</param>
		/// <param name="bAllowDuplicate">True to create a new TreeNode if there is already a TreeNode with this Text or false otherwise.</param>
		/// <param name="tnAdd">Out TreeNode that is added or null if no TreeNode is added.</param>
		/// <returns>True if the TreeNode was successfully added or false otherwise.</returns>
		private bool AddNodeInt(TreeNodeCollection tnc, string strText, bool bAllowDuplicate, out TreeNode tnAdd)
		{
			bool bRetVal = false;

			tnAdd = null;

			if(bAllowDuplicate)
			{
				tnAdd = tnc.Add(strText);

				bRetVal = true;
			}
			else
			{
				bool bExists = false;

				foreach(TreeNode tn in tnc)
				{
					if(tn.Text == strText)
					{
						bExists = true;
						break;
					}
				}

				if(!bExists)
				{
					tnAdd = tnc.Add(strText);

					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion AddNodeInt

		#endregion AddNode and AddNodeInt

		#endregion Public Main Methods



		#region Private Main Methods

		#region LabelEditing/NodeEditing (BeginEditMethod, BeginAllowLabelEditing and StartLabelEdit)

		#region BeginEditMethod

		/// <summary>
		/// Start editing TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to start editing.</param>
		private void BeginEditMethod(TreeNode tn)
		{
			tnLabelEdit = tn;
			tLabelEdit.Stop();
			tLabelEdit.Start();
		}

		#endregion BeginEditMethod



		#region BeginAllowLabelEditing

		/// <summary>
		/// Allow LabelEditing/NodeEditing.
		/// </summary>
		private void BeginAllowLabelEditing()
		{
			bLabelEditAllowed = false;
			tLabelEditAllowed.Stop();

			if(tLabelEdit.Interval != 0)
			{
				tLabelEditAllowed.Start();
			}
			else
			{
				StartLabelEdit();
			}
		}

		#endregion BeginAllowLabelEditing



		#region StartLabelEdit

		/// <summary>
		/// Start LabelEditing/NodeEditing if allowed.
		/// Note that the TreeNode variable tnLabelEdit is used.
		/// </summary>
		private void StartLabelEdit()
		{
			if(this.LabelEdit && tnLabelEdit != null && this.IsTreeNodeSelected(tnLabelEdit) && !bRubberbandIsOnScreen)
			{
				tnLabelEdit.BeginEdit();
				tnLabelEdit = null;
			}
		}

		#endregion StartLabelEdit

		#endregion LabelEditing/NodeEditing (BeginEditMethod, BeginAllowLabelEditing and StartLabelEdit)



		#region Rubberband Selection Expand/Collapse (BeginAllowRubberbandSelectionExpand and BeginAllowRubberbandSelectionCollapse)

		#region BeginAllowRubberbandSelectionExpand

		/// <summary>
		/// Allow rubberband selection after TreeNode has been Expanded.
		/// </summary>
		private void BeginAllowRubberbandSelectionExpand()
		{
			bExpandInProgress = true;
			tRubberbandSelectionAllowedExpand.Stop();

			if(tRubberbandSelectionAllowedExpand.Interval != 0)
			{
				tRubberbandSelectionAllowedExpand.Start();
			}
			else
			{
				bExpandInProgress = false;
			}
		}

		#endregion BeginAllowRubberbandSelectionExpand



		#region BeginAllowRubberbandSelectionCollapse

		/// <summary>
		/// Allow rubberband selection after TreeNode has been Collapsed.
		/// </summary>
		private void BeginAllowRubberbandSelectionCollapse()
		{
			bCollapseInProgress = true;
			tRubberbandSelectionAllowedCollapse.Stop();

			if(tRubberbandSelectionAllowedCollapse.Interval != 0)
			{
				tRubberbandSelectionAllowedCollapse.Start();
			}
			else
			{
				bCollapseInProgress = false;
			}
		}

		#endregion BeginAllowRubberbandSelectionCollapse

		#endregion Rubberband Selection Expand/Collapse (BeginAllowRubberbandSelectionExpand and BeginAllowRubberbandSelectionCollapse)

		#endregion Private Main Methods



		#region Public Help Methods

		#region Retrieval Public Help Methods

		#region GetSelectedMWTreeNodeWrapper

		/// <summary>
		/// Get the MWTreeNodeWrapper that contains the supplied TreeNode if it exists in the Selected MWTreeNodeWrapper Hashtable.
		/// </summary>
		/// <param name="tn">TreeNode for which to return an MWTreeNodeWrapper.</param>
		/// <returns>The MWTreeNodeWrapper that contains the supplied TreeNode if it exists in the Selected MWTreeNodeWrapper Hashtable or null otherwise.</returns>
		public MWTreeNodeWrapper GetSelectedMWTreeNodeWrapper(TreeNode tn)
		{
			MWTreeNodeWrapper mwtnw = null;

			if(IsTreeNodeSelected(tn))
			{
				mwtnw = htSelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;
			}

			return mwtnw;
		}

		#endregion GetSelectedMWTreeNodeWrapper



		#region GetDisabledMWTreeNodeWrapper

		/// <summary>
		/// Get the MWTreeNodeWrapper that contains the supplied TreeNode if it exists in the Disabled MWTreeNodeWrapper Hashtable.
		/// </summary>
		/// <param name="tn">TreeNode for which to return an MWTreeNodeWrapper.</param>
		/// <returns>The MWTreeNodeWrapper that contains the supplied TreeNode if it exists in the Disabled MWTreeNodeWrapper Hashtable or null otherwise.</returns>
		public MWTreeNodeWrapper GetDisabledMWTreeNodeWrapper(TreeNode tn)
		{
			MWTreeNodeWrapper mwtnw = null;

			if(IsTreeNodeDisabled(tn))
			{
				mwtnw = htDisabledTreeNodes[tn.GetHashCode()] as MWTreeNodeWrapper;
			}

			return mwtnw;
		}

		#endregion GetDisabledMWTreeNodeWrapper



		#region IsAnyChildTreeNodeSelected

		/// <summary>
		/// Checks if any Child TreeNode of the TreeNode supplied is selected.
		/// </summary>
		/// <param name="tn">TreeNode to check.</param>
		/// <returns>True if any Child TreeNode of the supplied TreeNode is selected or false otherwise.</returns>
		public bool IsAnyChildTreeNodeSelected(TreeNode tn)
		{
			bool bRetVal = false;
			int iLevel = GetTreeNodeLevel(tn);
			TreeNode tnGrandParent = GetTreeNodeGrandParent(tn);

			foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
			{
				int iLvl = GetTreeNodeLevel(mwtnw.Node);
				if(mwtnw.Node != tn && GetTreeNodeGrandParent(mwtnw.Node) == tnGrandParent && GetTreeNodeLevel(mwtnw.Node) > iLevel)
				{
					bRetVal = true;
					break;
				}
			}

			return bRetVal;
		}

		#endregion IsAnyChildTreeNodeSelected



		#region IsTreeNodeChecked

		/// <summary>
		/// Checks if the TreeNode supplied is checked or not.
		/// Note that passing in null as the TreeNode will result in false being returned.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if it is checked or not.</param>
		/// <returns>True if supplied TreeNode is checked or false otherwise.</returns>
		public bool IsTreeNodeChecked(TreeNode tn)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(this.CheckedNodes != null)
				{
					if(this.CheckedNodes.Contains(tn.GetHashCode()))
					{
						bRetVal = true;
					}
				}
			}

			return bRetVal;
		}

		#endregion IsTreeNodeChecked



		#region IsTreeNodeDisabled

		/// <summary>
		/// Checks if the TreeNode supplied is disabled or not.
		/// Note that passing in null as the TreeNode will result in false being returned.
		/// Note that being disabled means that BackColor and ForeColor has changed to give the TreeNode the appearance of being disabled.
		///		A whole TreeView can be disabled without the particular TreeNode supplied to this function returning true (as in being disabled).
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if it is disabled or not.</param>
		/// <returns>True if supplied TreeNode is disabled or false otherwise.</returns>
		public bool IsTreeNodeDisabled(TreeNode tn)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(htDisabledTreeNodes != null)
				{
					if(htDisabledTreeNodes.Contains(tn.GetHashCode()))
					{
						bRetVal = true;
					}
				}
			}

			return bRetVal;
		}

		#endregion IsTreeNodeDisabled



		#region IsTreeNodeSelected

		/// <summary>
		/// Checks if the TreeNode supplied is selected or not.
		/// Note that passing in null as the TreeNode will result in false being returned.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if it is selected or not.</param>
		/// <returns>True if supplied TreeNode is selected or false otherwise.</returns>
		public bool IsTreeNodeSelected(TreeNode tn)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(this.SelNodes != null)
				{
					if(this.SelNodes.Contains(tn.GetHashCode()))
					{
						bRetVal = true;
					}
				}
			}

			return bRetVal;
		}

		#endregion IsTreeNodeSelected

		#endregion Retrieval Public Help Methods



		#region Retrieval Regular Expression Public Help Methods

		#region IsCheckNodeRegExSatisfied

		/// <summary>
		/// Checks if the TreeNode's Text satisfies the regular expression or not, if it does the TreeNode can be checked otherwise not.
		/// </summary>
		/// <param name="strLabelText">TreeNode Text that should be checked against the regular expression in the CheckNodeRegEx property.</param>
		/// <returns>True if the TreeNode's Text satisfies the regular expression or false otherwise.</returns>
		public bool IsCheckNodeRegExSatisfied(string strText)
		{
			return Regex.IsMatch(strText, this.CheckNodeRegEx);
		}

		#endregion IsCheckNodeRegExSatisfied



		#region IsDisallowLabelEditRegExSatisfied

		/// <summary>
		/// Checks if the proposed Label Text satisfies the regular expression or not. If it does, do not allow this TreeNode's Text to be
		///		edited (done in the OnBeforeLabelEdit EventHandler).
		/// </summary>
		/// <param name="strLabelText">Label Text that should be checked against the regular expression in the DisallowLabelEditRegEx property.</param>
		/// <returns>True if the proposed Label Text satisfies the regular expression or false otherwise.</returns>
		public bool IsDisallowLabelEditRegExSatisfied(string strLabelText)
		{
			bool bRetVal = true;

			if(this.DisallowLabelEditRegEx == string.Empty)
			{
				bRetVal = false;
			}
			else
			{
				bRetVal = Regex.IsMatch(strLabelText, this.DisallowLabelEditRegEx);
			}

			return bRetVal;
		}

		#endregion IsDisallowLabelEditRegExSatisfied



		#region IsLabelEditRegExSatisfied

		/// <summary>
		/// Checks if the proposed Label Text satisfies the regular expression or not.
		/// Note that a Label's Text could still be blank if the AllowBlankNodeText property is set to true.
		/// </summary>
		/// <param name="strLabelText">Label Text that should be checked against the regular expression in the LabelEditRegEx property.</param>
		/// <returns>True if the proposed Label Text satisfies the regular expression or false otherwise.</returns>
		public bool IsLabelEditRegExSatisfied(string strLabelText)
		{
			if(strLabelText != null)
			{
				return Regex.IsMatch(strLabelText, this.LabelEditRegEx);
			}
			else
			{
				return false;
			}
		}

		#endregion IsLabelEditRegExSatisfied



		#region IsSelectNodeRegExSatisfied

		/// <summary>
		/// Checks if the TreeNode's Text satisfies the regular expression or not, if it does the TreeNode can be selected otherwise not.
		/// </summary>
		/// <param name="strLabelText">TreeNode Text that should be checked against the regular expression in the SelectNodeRegEx property.</param>
		/// <returns>True if the TreeNode's Text satisfies the regular expression or false otherwise.</returns>
		public bool IsSelectNodeRegExSatisfied(string strText)
		{
			return Regex.IsMatch(strText, this.SelectNodeRegEx);
		}

		#endregion IsSelectNodeRegExSatisfied

		#endregion Retrieval Regular Expression Public Help Methods



		#region Modification Public Help Methods

		#region ActivateOrDeactivateSelNodes

		/// <summary>
		/// Fixes activation and/or deactivation for TreeNodes.
		/// </summary>
		public void ActivateOrDeactivateSelNodes()
		{
			if(!this.HideSelection)
			{
				if(bActive)
				{
					ActivateSelNodes();
				}
				else
				{
					DeactivateSelNodes();
				}
			}
			else
			{
				if(!bActive)
				{
					ForceDeactivateSelNodes();
				}
			}
		}

		#endregion ActivateOrDeactivateSelNodes



		#region CheckAllNodes

		/// <summary>
		/// Check all TreeNodes in this MWTreeView.
		/// </summary>
		public void CheckAllNodes()
		{
			foreach(TreeNode tn in this.Nodes)
			{
				CheckAllNodes(tn);
				CheckNode(tn, true);
			}
		}

		/// <summary>
		/// Check all TreeNodes in the Nodes collection of the TreeNode supplied.
		/// </summary>
		/// <param name="tn"></param>
		public void CheckAllNodes(TreeNode tn)
		{
			foreach(TreeNode tn2 in tn.Nodes)
			{
				CheckAllNodes(tn2);
				CheckNode(tn2, true);
			}
		}

		#endregion CheckAllNodes



		#region CheckBranch, UncheckBranch and ToggleCheckBranch

		#region CheckBranch

		/// <summary>
		/// Check all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the TreeNode supplied doesn't get checked itself.
		/// Note that no TreeNode is excluded from being checked.
		/// </summary>
		/// <param name="tn">TreeNode to check all child TreeNodes of.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be checked or false if only one level of child TreeNodes should be checked.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes got checked or false otherwise.</returns>
		public bool CheckBranch(TreeNode tn, bool bChangeSubBranches, bool bUpdate)
		{
			return CheckBranch(tn, bChangeSubBranches, null, bUpdate);
		}

		/// <summary>
		/// Check all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the TreeNode supplied doesn't get checked itself.
		/// </summary>
		/// <param name="tn">TreeNode to check all child TreeNodes of.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be checked or false if only one level of child TreeNodes should be checked.</param>
		/// <param name="tnExcluded">TreeNode that should be excluded from being unchecked.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes got checked or false otherwise.</returns>
		public bool CheckBranch(TreeNode tn, bool bChangeSubBranches, TreeNode tnExcluded, bool bUpdate)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				foreach(TreeNode tn2 in tn.Nodes)
				{
					if(tnExcluded != tn2)
					{
						CheckNode(tn2, bUpdate);
					}

					if(bChangeSubBranches)
					{
						CheckBranch(tn2, bChangeSubBranches, bUpdate);
					}
				}

				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion CheckBranch



		#region UncheckBranch

		/// <summary>
		/// Uncheck all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the TreeNode supplied doesn't get unchecked itself.
		/// Note that no TreeNode is excluded from unchecked.
		/// </summary>
		/// <param name="tn">TreeNode to uncheck all child TreeNodes of.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be unchecked or false if only one level of child TreeNodes should be unchecked.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes got unchecked or false otherwise.</returns>
		public bool UncheckBranch(TreeNode tn, bool bChangeSubBranches, bool bUpdate)
		{
			return UncheckBranch(tn, bChangeSubBranches, null, bUpdate);
		}

		/// <summary>
		/// Uncheck all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the TreeNode supplied doesn't get unchecked itself.
		/// </summary>
		/// <param name="tn">TreeNode to uncheck all child TreeNodes of.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be unchecked or false if only one level of child TreeNodes should be unchecked.</param>
		/// <param name="tnExcluded">TreeNode that should be excluded from being unchecked.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes got unchecked or false otherwise.</returns>
		public bool UncheckBranch(TreeNode tn, bool bChangeSubBranches, TreeNode tnExcluded, bool bUpdate)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				foreach(TreeNode tn2 in tn.Nodes)
				{
					if(tnExcluded != tn2)
					{
						UncheckNode(tn2, bUpdate);
					}

					if(bChangeSubBranches)
					{
						UncheckBranch(tn2, bChangeSubBranches, bUpdate);
					}
				}

				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion UncheckBranch



		#region ToggleCheckBranch

		/// <summary>
		/// Toggle the Checked property of all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the Checked property of the TreeNode supplied doesn't get toggled.
		/// Note that no TreeNode is excluded from having its Checked property toggled.
		/// </summary>
		/// <param name="tn">TreeNode whose child TreeNode's Checked property should be toggled.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches' Checked property should be toggled or false if only one level of child TreeNodes should have their Checked property toggled.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes' Checked property got toggled or false otherwise.</returns>
		public bool ToggleCheckBranch(TreeNode tn, bool bChangeSubBranches, bool bUpdate)
		{
			return ToggleCheckBranch(tn, bChangeSubBranches, null, bUpdate);
		}

		/// <summary>
		/// Toggle the Checked property of all child TreeNodes (Nodes collection) of the TreeNode supplied.
		/// Note that the Checked property of the TreeNode supplied doesn't get toggled.
		/// </summary>
		/// <param name="tn">TreeNode whose child TreeNode's Checked property should be toggled.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches' Checked property should be toggled or false if only one level of child TreeNodes should have their Checked property toggled.</param>
		/// <param name="tnExcluded">TreeNode that should be excluded from having its Checked property toggled.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if TreeNodes' Checked property got toggled or false otherwise.</returns>
		public bool ToggleCheckBranch(TreeNode tn, bool bChangeSubBranches, TreeNode tnExcluded, bool bUpdate)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				foreach(TreeNode tn2 in tn.Nodes)
				{
					if(tnExcluded != tn2)
					{
						ToggleCheckNode(tn2, bUpdate);
					}

					if(bChangeSubBranches)
					{
						ToggleCheckBranch(tn2, bChangeSubBranches, bUpdate);
					}
				}

				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion ToggleCheckBranch

		#endregion CheckBranch, UncheckBranch and ToggleCheckBranch



		#region CheckNode, UncheckNode and ToggleCheckNode

		#region CheckNode

		/// <summary>
		/// Check the TreeNode supplied (set the Checked property to true).
		/// </summary>
		/// <param name="tn">TreeNode to check (set the Checked property to true).</param>
		/// <returns>True if the TreeNodes was checked (Checked property set to true) or false otherwise.</returns>
		public bool CheckNode(TreeNode tn)
		{
			return CheckNode(tn, true);
		}

		/// <summary>
		/// Check the TreeNode supplied (set the Checked property to true).
		/// </summary>
		/// <param name="tn">TreeNode to check (set the Checked property to true).</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if the TreeNodes was checked (Checked property set to true) or false otherwise.</returns>
		public bool CheckNode(TreeNode tn, bool bUpdate)
		{
			bool bRetVal = false;

			bool bOldChecked = tn.Checked;

			if(tn != null)
			{
				bool bOldForceCheckNode = bForceCheckNode;

				bForceCheckNode = true;

				if(this.AllowMultiCheck)
				{
					if(IsTreeNodeChecked(tn))
					{
						if(!tn.Checked && bUpdate)
						{
							tn.Checked = true;
						}
					}
					else
					{
						if(!tn.Checked)
						{
							if(bUpdate)
							{
								tn.Checked = true;
							}

							if(!IsTreeNodeChecked(tn) && IsCheckNodeRegExSatisfied(tn.Text))
							{
								this.CheckedNodesAdd(tn);
							}
						}
					}
				}
				else
				{
					ClearCheckedNodes();

					if(bUpdate)
					{
						tn.Checked = true;
					}

					if(!IsTreeNodeChecked(tn) && IsCheckNodeRegExSatisfied(tn.Text))
					{
						this.CheckedNodesAdd(tn);
					}
				}

				if(bOldChecked != tn.Checked)
				{
					bRetVal = true;
				}

				bForceCheckNode = bOldForceCheckNode;
			}

			return bRetVal;
		}

		#endregion CheckNode



		#region UncheckNode

		/// <summary>
		/// Uncheck the TreeNode supplied (set the Checked property to false).
		/// </summary>
		/// <param name="tn">TreeNode to uncheck (set the Checked property to false).</param>
		/// <returns>True if the TreeNodes was unchecked (Checked property set to false) or false otherwise.</returns>
		public bool UncheckNode(TreeNode tn)
		{
			return UncheckNode(tn, true);
		}

		/// <summary>
		/// Uncheck the TreeNode supplied (set the Checked property to false).
		/// </summary>
		/// <param name="tn">TreeNode to uncheck (set the Checked property to false).</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if the TreeNodes was unchecked (Checked property set to false) or false otherwise.</returns>
		public bool UncheckNode(TreeNode tn, bool bUpdate)
		{
			bool bRetVal = false;

			bool bOldChecked = tn.Checked;

			if(tn != null)
			{
				bool bOldForceCheckNode = bForceCheckNode;

				bForceCheckNode = true;

				if(IsTreeNodeChecked(tn))
				{
					if(tn.Checked)
					{
						if(bUpdate)
						{
							tn.Checked = false;
						}

						this.CheckedNodesRemove(tn);
					}
				}
				else
				{
					if(tn.Checked && bUpdate)
					{
						tn.Checked = false;
					}
				}

				if(bOldChecked != tn.Checked)
				{
					bRetVal = true;
				}

				bForceCheckNode = bOldForceCheckNode;
			}

			return bRetVal;
		}

		#endregion UncheckNode



		#region ToggleCheckNode

		/// <summary>
		/// Toggle the Checked property of the TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to toggle the Checked property of.</param>
		/// <returns>True if the Checked property has been toggled or false otherwise.</returns>
		public bool ToggleCheckNode(TreeNode tn)
		{
			return ToggleCheckNode(tn, true);
		}

		/// <summary>
		/// Toggle the Checked property of the TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to toggle the Checked property of.</param>
		/// <param name="bUpdate">True if the TreeNode's Checked property should be updated or false otherwise.</param>
		/// <returns>True if the Checked property has been toggled or false otherwise.</returns>
		public bool ToggleCheckNode(TreeNode tn, bool bUpdate)
		{
			bool bRetVal = false;

			bool bOldChecked = tn.Checked;

			if(tn != null)
			{
				if(tn.Checked)
				{
					UncheckNode(tn, bUpdate);
				}
				else
				{
					CheckNode(tn, bUpdate);
				}

				if(bOldChecked != tn.Checked)
				{
					bRetVal = true;
				}
			}

			return bRetVal;
		}

		#endregion ToggleCheckNode

		#endregion CheckNode, UncheckNode and ToggleCheckNode



		#region ClearCheckedNodes

		/// <summary>
		/// Clear all TreeNodes in the CheckedNodes property.
		/// </summary>
		public void ClearCheckedNodes()
		{
			if(this.CheckedNodes != null)
			{
				int iMax = this.CheckedNodes.Count;

				for(int i = 0; i < iMax; i++)
				{
					foreach(TreeNode tn in this.CheckedNodes.Values)
					{
						if(this.CheckedNodesRemove(tn))
						{
							tn.Checked = false;
							break;
						}
					}
				}
			}
			else
			{
				this.CheckedNodes = new Hashtable();
			}
		}

		#endregion ClearCheckedNodes



		#region ClearSelBranch

		/// <summary>
		/// Clear SelNodes in a Branch.
		/// Note that the TreeNode supplied is also cleared.
		/// </summary>
		/// <param name="tn">TreeNode whose whole Branch should be cleared.</param>
		public void ClearSelBranch(TreeNode tn)
		{
			if(this.SelNodes != null)
			{
				ArrayList al = new ArrayList();

				foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
				{
					TreeNode tnGrandParent = GetTreeNodeGrandParent(mwtnw.Node);

					if(tnGrandParent == tn)
					{
						al.Add(mwtnw);
					}
				}

				for(int i = 0; i < al.Count; i++)
				{
					if((al[i] as MWTreeNodeWrapper).Node == this.SelNode)
					{
						this.SelNode = null;
					}

					this.SelNodesRemove((al[i] as MWTreeNodeWrapper).Node);
				}
			}
			else
			{
				this.SelNodes = new Hashtable();
			}
		}

		#endregion ClearSelBranch



		#region ClearSelNodes

		#region ClearSelNodes (clear all SelNodes)

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them.
		/// </summary>
		public void ClearSelNodes()
		{
			ClearSelNodes(true);
		}

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them.
		/// </summary>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		protected void ClearSelNodes(bool bTriggerSelNodesAddRemoveEvents)
		{
			if(this.SelNodes != null)
			{
				int iMax = this.SelNodes.Count;

				for(int i = 0; i < iMax; i++)
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						if(this.SelNodesRemove(mwtnw.Node, bTriggerSelNodesAddRemoveEvents))
						{
							break;
						}
					}
				}
			}
			else
			{
				this.SelNodes = new Hashtable();
			}
		}

		#endregion ClearSelNodes (clear all SelNodes)



		#region ClearSelNodes (clear all SelNodes except one)

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them, except for the tn TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode not to deselect.</param>
		public void ClearSelNodes(TreeNode tn)
		{
			ClearSelNodes(tn, true);
		}

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them, except for the tn TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode not to deselect.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		public void ClearSelNodes(TreeNode tn, bool bTriggerSelNodesAddRemoveEvents)
		{
			if(tn == null)
			{
				ClearSelNodes(bTriggerSelNodesAddRemoveEvents);
			}
			else if(this.SelNodes != null)
			{
				int iMax = this.SelNodes.Count;

				for(int i = 0; i < iMax; i++)
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						if(mwtnw.Node != tn)
						{
							if(this.SelNodesRemove(mwtnw.Node, bTriggerSelNodesAddRemoveEvents))
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				this.SelNodes = new Hashtable();
			}
		}

		#endregion ClearSelNodes (clear all SelNodes except one)



		#region ClearSelNodes (clear all SelNodes except a range)

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them, except for the visible TreeNodes in the range between, and
		///		including, the two TreeNodes supplied.
		/// </summary>
		/// <param name="tnFrom">First TreeNode in range of TreeNodes that should not be deselected.</param>
		/// <param name="tnTo">Last TreeNode in range of TreeNodes that should not be deselected.</param>
		public void ClearSelNodes(TreeNode tnFrom, TreeNode tnTo)
		{
			ClearSelNodes(tnFrom, tnTo, true, true);
		}

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them, except for the TreeNodes in the range between, and including, the
		///		two TreeNodes supplied.
		/// </summary>
		/// <param name="tnFrom">First TreeNode in range of TreeNodes that should not be deselected.</param>
		/// <param name="tnTo">Last TreeNode in range of TreeNodes that should not be deselected.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		public void ClearSelNodes(TreeNode tnFrom, TreeNode tnTo, bool bTriggerSelNodesAddRemoveEvents)
		{
			ClearSelNodes(tnFrom, tnTo, bTriggerSelNodesAddRemoveEvents, true);
		}

		/// <summary>
		/// Clear all TreeNodes in the SelNodes property and deselect them, except for the TreeNodes in the range between, and including, the
		///		two TreeNodes supplied.
		/// </summary>
		/// <param name="tnFrom">First TreeNode in range of TreeNodes that should not be deselected.</param>
		/// <param name="tnTo">Last TreeNode in range of TreeNodes that should not be deselected.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		/// <param name="bOnlyVisible">True if only visible TreeNodes should be considered to be part of the range or false if both visible and non-visible TreeNodes should be considered to be part of the range.</param>
		public void ClearSelNodes(TreeNode tnFrom, TreeNode tnTo, bool bTriggerSelNodesAddRemoveEvents, bool bOnlyVisible)
		{
			if(tnFrom == tnTo)
			{
				if(tnFrom != null)
				{
					ClearSelNodes(tnFrom, bTriggerSelNodesAddRemoveEvents);
				}
				else
				{
					ClearSelNodes(bTriggerSelNodesAddRemoveEvents);
				}
			}
			else if(this.SelNodes != null)
			{
				if(tnTo == null)
				{
					tnTo = this.Nodes[this.Nodes.Count - 1];

					if(bOnlyVisible)
					{
						while(tnTo.NextVisibleNode != null)
						{
							tnTo = tnTo.NextVisibleNode;
						}
					}
					else
					{
						while(tnTo.NextNode != null)
						{
							tnTo = tnTo.NextNode;
						}
					}
				}

				Hashtable ht = new Hashtable();

				TreeNode tn = tnFrom;

				while(tn != null && tn != tnTo)
				{
					ht.Add(tn.GetHashCode(), tn);

					if(bOnlyVisible)
					{
						tn = tn.NextVisibleNode;
					}
					else
					{
						tn = tn.NextNode;
					}
				}

				ht.Add(tnTo.GetHashCode(), tnTo);

				int iMax = this.SelNodes.Count;

				for(int i = 0; i < iMax; i++)
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						if(!ht.Contains(mwtnw.Node.GetHashCode()))
						{
							if(this.SelNodesRemove(mwtnw.Node, bTriggerSelNodesAddRemoveEvents))
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				this.SelNodes = new Hashtable();
			}
		}

		#endregion ClearSelNodes (clear all SelNodes except a range)



		#region ClearSelNodes (force-clear all SelNodes)

		/// <summary>
		/// Force-clear all TreeNodes in the SelNodes property and deselect them.
		/// Even if there are EventHandlers setting their MWCancelEventArgs.Cancel to true the SelNodes are cleared.
		/// </summary>
		private void ClearSelNodesForce()
		{
			if(this.SelNodes != null)
			{
				foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
				{
					MWTreeNodeWrapper.Deselect(mwtnw);
				}

				this.SelNodes.Clear();
			}
			else
			{
				this.SelNodes = new Hashtable();
			}
		}

		#endregion ClearSelNodes (force-clear all SelNodes)

		#endregion ClearSelNodes



		#region EnsureSelNodeNotNull

		/// <summary>
		/// If SelNode is null a TreeNode from the SelNodes HashTable gets selected (SelNode is set to the first TreeNode returned from the
		///		SelNodes HashTable). If the SelNodes HashTable is empty and AllowNoSelNode is false a TreeNode is selected (SelNode is set
		///		to the first TreeNode for the TreeView if there is one).
		/// </summary>
		/// <returns>True if SelNode is not null or if it was changed or false otherwise.</returns>
		public bool EnsureSelNodeNotNull()
		{
			bool bRetVal = false;

			if(this.SelNode == null)
			{
				if(this.SelNodes != null)
				{
					if(this.SelNodes.Count > 0)
					{
						if(this.SelNode == null)
						{
							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								bRetVal = true;
								SelectNode(mwtnw.Node, true);
								break;
							}
						}
					}
					else if(this.Nodes.Count > 0 && !this.AllowNoSelNode)
					{
						bRetVal = true;
						SelectNode(this.Nodes[0], true);
					}
				}
			}
			else
			{
				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion EnsureSelNodeNotNull



		#region HiglightNode, LowlightNode and DisablelightNode

		#region HiglightNode

		/// <summary>
		/// Highlights a TreeNode as active.
		/// </summary>
		/// <param name="tn">TreeNode to highlight.</param>
		public void HighlightNode(TreeNode tn)
		{
			HighlightNode(tn, bActive);
		}

		/// <summary>
		/// Highlights TreeNode supplied as active or inactive.
		/// </summary>
		/// <param name="tn">TreeNode to highlight.</param>
		/// <param name="bTreeViewActive">True if TreeNode should be highlighted as active or false if it should be highlighted as inactive.</param>
		public void HighlightNode(TreeNode tn, bool bTreeViewActive)
		{
			if(this.Enabled)
			{
				if(bTreeViewActive)
				{
					if(this.UseExtendedSelectionColors)
					{
						if(tn.BackColor != this.SelectedTreeNodeBackColor)
						{
							tn.BackColor = this.SelectedTreeNodeBackColor;
						}

						if(tn.ForeColor != this.SelectedTreeNodeForeColor)
						{
							tn.ForeColor = this.SelectedTreeNodeForeColor;
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
					if(tn.BackColor != SystemColors.Control)
					{
						tn.BackColor = SystemColors.Control;
					}

					if(tn.ForeColor != this.ForeColor)
					{
						tn.ForeColor = this.ForeColor;
					}
				}
			}
		}

		#endregion HiglightNode



		#region LowlightNode

		/// <summary>
		/// Removes highlight from supplied TreeNode: 'Lowlight'.
		/// </summary>
		/// <param name="tn">TreeNode to remove highlight from.</param>
		public void LowlightNode(TreeNode tn)
		{
			if(this.Enabled)
			{
				if(tn.BackColor != this.BackColor)
				{
					tn.BackColor = this.BackColor;
				}

				if(tn.ForeColor != this.ForeColor)
				{
					tn.ForeColor = this.ForeColor;
				}
			}
		}

		#endregion LowlightNode



		#region DisablelightNode

		/// <summary>
		/// Change the BackColor for a TreeNode to look like it is disabled.
		/// </summary>
		/// <param name="tn">TreeNode to change the BackColor of so that it will look disabled.</param>
		public void DisablelightNode(TreeNode tn)
		{
			if(this.UseExtendedDisabledColors)
			{
				if(tn.BackColor != this.DisabledTreeNodeBackColor)
				{
					tn.BackColor = this.DisabledTreeNodeBackColor;
				}

				if(tn.ForeColor != this.DisabledTreeNodeForeColor)
				{
					tn.ForeColor = this.DisabledTreeNodeForeColor;
				}
			}
		}

		#endregion DisablelightNode

		#endregion HiglightNode, LowlightNode and DisablelightNode



		#region RemoveNode, RemoveNodes, RemoveSelectedNodes and DeleteNode

		#region RemoveNode

		/// <summary>
		/// Remove one TreeNode contained in an MWTreeNodeWrapper. The TreeNode is also removed from the SelNodes property, the CheckedNodes
		///		property and/or the SelNode property.
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper to remove.</param>
		/// <returns>True if the TreeNode in the MWTreeNodeWrapper was removed or false otherwise.</returns>
		public bool RemoveNode(MWTreeNodeWrapper mwtnw)
		{
			bool bRetVal = false;

			if(mwtnw != null)
			{
				bRetVal = RemoveNode(mwtnw.Node);
			}

			return bRetVal;
		}

		/// <summary>
		/// Remove one TreeNode. The TreeNode is also removed from the SelNodes property, the CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to remove.</param>
		/// <returns>True if the TreeNode was removed or false otherwise.</returns>
		public bool RemoveNode(TreeNode tn)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				DeleteNode(tn);

				try
				{
					tn.Remove();

					if(tn.TreeView == null && !IsTreeNodeSelected(tn) && !IsTreeNodeChecked(tn) && tn != this.SelNode)
					{
						bRetVal = true;
					}
				}
				catch
				{
				}
			}

			return bRetVal;
		}

		#endregion RemoveNode



		#region RemoveNodes

		/// <summary>
		/// Remove an array of TreeNodes. The TreeNodes are also removed from the SelNodes property, the CheckedNodes property and/or the
		///		SelNode property.
		/// </summary>
		/// <param name="atn">TreeNode array to remove.</param>
		/// <returns>True if at least one TreeNode was removed or false if no TreeNodes were removed.</returns>
		public bool RemoveNodes(TreeNode[] atn)
		{
			bool bRetVal = false;

			if(atn != null)
			{
				foreach(TreeNode tn in atn)
				{
					if(RemoveNode(tn))
					{
						bRetVal = true;
					}
				}
			}

			return bRetVal;
		}

		/// <summary>
		/// Remove an array of TreeNodes contained in MWTreeNodeWrappers. The TreeNodes are also removed from the SelNodes property, the
		///		CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="amwtnw">MWTreeNodeWrapper array whose TreeNodes should be removed.</param>
		/// <returns>True if at least one TreeNode from the MWTreeNodeWrappers was removed or false if no TreeNodes were removed.</returns>
		public bool RemoveNodes(MWTreeNodeWrapper[] amwtnw)
		{
			bool bRetVal = false;

			if(amwtnw != null)
			{
				foreach(MWTreeNodeWrapper mwtnw in amwtnw)
				{
					if(RemoveNode(mwtnw))
					{
						bRetVal = true;
					}
				}
			}

			return bRetVal;
		}

		/// <summary>
		/// Remove an ArrayList of TreeNodes or an ArrayList of TreeNodes contained in MWTreeNodeWrappers. The TreeNodes are also removed from
		///		the SelNodes property, the CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="altn">ArrayList containing TreeNodes that should be removed or MWTreeNodeWrappers whose TreeNodes should be removed.</param>
		/// <returns>True if at least one TreeNode or one TreeNode from the MWTreeNodeWrappers was removed or false if no TreeNodes were removed.</returns>
		public bool RemoveNodes(ArrayList altn)
		{
			bool bRetVal = false;

			if(altn != null)
			{
				for(int i = 0; i < altn.Count; i++)
				{
					if(altn[i] is TreeNode)
					{
						if(RemoveNode(altn[i] as TreeNode))
						{
							bRetVal = true;
						}
					}
					else if(altn[i] is MWTreeNodeWrapper)
					{
						if(RemoveNode(altn[i] as MWTreeNodeWrapper))
						{
							bRetVal = true;
						}
					}
				}
			}

			return bRetVal;
		}

		/// <summary>
		/// Remove a Hashtable of TreeNodes or a Hashtable of TreeNodes contained in MWTreeNodeWrappers. The TreeNodes are also removed from
		///		the SelNodes property, the CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="altn">Hashtable containing TreeNodes that should be removed or MWTreeNodeWrappers whose TreeNodes should be removed.</param>
		/// <returns>True if at least one TreeNode or one TreeNode from the MWTreeNodeWrappers was removed or false if no TreeNodes were removed.</returns>
		public bool RemoveNodes(Hashtable httn)
		{
			bool bRetVal = false;

			if(httn != null && httn.Count > 0)
			{
				ArrayList al = new ArrayList();

				foreach(object tn in httn.Values)
				{
					if(tn is TreeNode || tn is MWTreeNodeWrapper)
					{
						al.Add(tn);
					}
				}

				if(al.Count > 0)
				{
					bRetVal = this.RemoveNodes(al);
				}
			}

			return bRetVal;
		}

		#endregion RemoveNodes



		#region RemoveSelectedNodes

		/// <summary>
		/// Remove all selected TreeNodes. The TreeNodes are also removed from the SelNodes property, the CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="bRemoveTreeNodeMouseDownIfNotSelected">True if the TreeNodeMouseDown TreeNode should be removed even if it is not part of the SelNodes property or false to only remove it if is part of the SelNodes property.</param>
		/// <returns>True if at least one TreeNode was removed or false if no TreeNodes were removed.</returns>
		public bool RemoveSelectedNodes(bool bRemoveTreeNodeMouseDownIfNotSelected)
		{
			bool bRetVal = false;

			if(bRemoveTreeNodeMouseDownIfNotSelected)
			{
				bRetVal = this.RemoveNode(this.TreeNodeMouseDown);
			}

			if(RemoveNodes(this.SelNodes))
			{
				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion RemoveSelectedNodes



		#region DeleteNode

		/// <summary>
		/// Delete the supplied TreeNode from the SelNodes property, the CheckedNodes property and/or the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to delete.</param>
		private void DeleteNode(TreeNode tn)
		{
			foreach(TreeNode tnChild in tn.Nodes)
			{
				DeleteNode(tnChild);
			}

			if(tn != null)
			{
				if(IsTreeNodeSelected(tn))
				{
					if(this.SelNodes != null)
					{
						this.SelNodes.Remove(tn.GetHashCode());
					}
				}

				if(IsTreeNodeChecked(tn))
				{
					if(this.CheckedNodes != null)
					{
						this.CheckedNodes.Remove(tn.GetHashCode());
					}
				}

				if(this.SelNode != null && this.SelNode == tn)
				{
					this.SelNode = null;
				}
			}
		}

		#endregion DeleteNode

		#endregion RemoveNode, RemoveNodes, RemoveSelectedNodes and DeleteNode



		#region SelectAllNodes

		#region SelectAllNodes (all nodes)

		/// <summary>
		/// Select all TreeNodes in this MWTreeView.
		/// </summary>
		public void SelectAllNodes()
		{
			foreach(TreeNode tn in this.Nodes)
			{
				SelectAllNodes(tn);
				SelectNode(tn, false);
			}

			SelectNode(this.Nodes[0], true);
		}

		#endregion SelectAllNodes (all nodes)



		#region SelectAllNodes (all nodes in a Nodes Collection)

		/// <summary>
		/// Select all TreeNodes in the Nodes collection of the TreeNode supplied.
		/// </summary>
		/// <param name="tn"></param>
		public void SelectAllNodes(TreeNode tn)
		{
			foreach(TreeNode tn2 in tn.Nodes)
			{
				SelectAllNodes(tn2);
				SelectNode(tn2, false);
			}
		}

		#endregion SelectAllNodes (all nodes in a Nodes Collection)



		#region SelectAllNodes (all nodes of a certain level)

		/// <summary>
		/// Select all TreeNodes in the Nodes collection of this Control if they are of the supplied TreeNodeLevel.
		/// When a TreeNodeLevel greater than the one supplied is encountered stop iterating.
		/// </summary>
		/// <param name="iLevel">TreeNodeLevel that TreeNodes must be in order to be selected.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when selecting TreeNodes.</param>
		public void SelectAllNodes(int iLevel, bool bChangeSelNode)
		{
			foreach(TreeNode tn in this.Nodes)
			{
				if(GetTreeNodeLevel(tn) == iLevel)
				{
					SelectNode(tn, bChangeSelNode);
				}
				else
				{
					if(GetTreeNodeLevel(tn) < iLevel)
					{
						SelectAllNodes(tn, iLevel, bChangeSelNode);
					}
					else
					{
						break;
					}
				}
			}
		}

		#endregion SelectAllNodes (all nodes of a certain level)



		#region SelectAllNodes (all nodes of a certain level in a Nodes Collection)

		/// <summary>
		/// Select all TreeNodes in the Nodes collection of the TreeNode supplied if they are of the supplied TreeNodeLevel.
		/// When a TreeNodeLevel greater than the one supplied is encountered stop iterating.
		/// </summary>
		/// <param name="tn">TreeNode in whose Nodes collection TreeNodes should be selected.</param>
		/// <param name="iLevel">TreeNodeLevel that TreeNodes must be in order to be selected.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when selecting TreeNodes.</param>
		public void SelectAllNodes(TreeNode tn, int iLevel, bool bChangeSelNode)
		{
			foreach(TreeNode tn2 in tn.Nodes)
			{
				if(GetTreeNodeLevel(tn2) == iLevel)
				{
					SelectNode(tn2, bChangeSelNode);
				}
				else
				{
					if(GetTreeNodeLevel(tn2) < iLevel)
					{
						SelectAllNodes(tn2, iLevel, bChangeSelNode);
					}
					else
					{
						break;
					}
				}
			}
		}

		#endregion SelectAllNodes (all nodes of a certain level in a Nodes Collection)

		#endregion SelectAllNodes



		#region SelectAllParentNodes, DeselectAllParentNodes

		#region SelectAllParentNodes

		/// <summary>
		/// Select supplied TreeNode and all its Parents.
		/// </summary>
		/// <param name="tn">TreeNode whose Parents should be selected.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when selecting TreeNodes.</param>
		public void SelectAllParentNodes(TreeNode tn, bool bChangeSelNode)
		{
			SelectNode(tn, bChangeSelNode);

			if(tn != null && tn.Parent != null)
			{
				SelectAllParentNodes(tn.Parent, bChangeSelNode);
			}
		}

		#endregion SelectAllParentNodes



		#region DeselectAllParentNodes

		/// <summary>
		/// Deselect supplied TreeNode and all its Parents.
		/// </summary>
		/// <param name="tn">TreeNode whose Parents should be deselected.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when deselecting TreeNodes.</param>
		public void DeselectAllParentNodes(TreeNode tn, bool bChangeSelNode)
		{
			DeselectNode(tn, bChangeSelNode);

			if(tn != null && tn.Parent != null)
			{
				DeselectAllParentNodes(tn.Parent, bChangeSelNode);
			}
		}

		#endregion DeselectAllParentNodes

		#endregion SelectAllParentNodes, DeselectAllParentNodes



		#region SelectBranch, DeselectBranch

		#region SelectBranch and SelectBranchIterative

		/// <summary>
		/// Select a branch of TreeNodes.
		/// The TreeNode supplied and the TreeNodes under it, if any (the TreeNodes in its Nodes collection), are selected.
		/// If bChangeSubBranches is true all the branches of the branches etc of the TreeNode supplied are selected.
		/// </summary>
		/// <param name="tn">TreeNode whose branch (TreeNodes in its Nodes collection) should be selected.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be deselected or false if only one level of child TreeNodes should be deselected.</param>
		/// <param name="bTopSelNode">True if the TreeNode passed in should be selected first and then all the child TreeNodes or false if all child TreeNodes should be selected and then the TreeNode supplied.\nThis should be true if TreeNodes are selected top-to-bottom and false if TreeNodes are selected bottom-to-top.</param>
		/// <param name="bchangeSelNode">True if the SelNode property should be changed when selecting the TreeNodes. Note that the SelNode property will only be set to the TreeNode passed in as the first argument to this method, not the TreeNodes passed in to calls to the SelectBranchIterative method.</param>
		/// <returns>True if the TreeNode itself and/or its child TreeNodes got selected.</returns>
		public bool SelectBranch(TreeNode tn, bool bChangeSubBranches, bool bTopSelNode, bool bchangeSelNode)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(!bTopSelNode)
				{
					SelectNode(tn, bchangeSelNode);
				}

				foreach(TreeNode tn2 in tn.Nodes)
				{
					if(bChangeSubBranches)
					{
						SelectBranchIterative(tn2, bChangeSubBranches, bTopSelNode);
					}
					else
					{
						SelectNode(tn2, false);
					}
				}

				if(bTopSelNode)
				{
					SelectNode(tn, bchangeSelNode);
				}

				bRetVal = true;
			}

			return bRetVal;
		}

		/// <summary>
		/// Note that this method should never be called except from the SelectBranch method.
		/// 
		/// Select a branch of TreeNodes.
		/// The TreeNode supplied and the TreeNodes under it, if any (the TreeNodes in its Nodes collection), are selected.
		/// If bChangeSubBranches is true all the branches of the branches etc of the TreeNode supplied are selected.
		/// </summary>
		/// <param name="tn">TreeNode whose branch (TreeNodes in its Nodes collection) should be selected.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be deselected or false if only one level of child TreeNodes should be deselected.</param>
		/// <param name="bTopSelNode">True if the TreeNode passed in should be selected first and then all the child TreeNodes or false if all child TreeNodes should be selected and then the TreeNode supplied.\nThis should be true if TreeNodes are selected top-to-bottom and false if TreeNodes are selected bottom-to-top.</param>
		/// <returns>True if the TreeNode itself and/or its child TreeNodes got selected.</returns>
		private bool SelectBranchIterative(TreeNode tn, bool bChangeSubBranches, bool bTopSelNode)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(!bTopSelNode)
				{
					SelectNode(tn, false);
				}

				foreach(TreeNode tn2 in tn.Nodes)
				{
					if(bChangeSubBranches)
					{
						SelectBranchIterative(tn2, bChangeSubBranches, bTopSelNode);
					}
					else
					{
						SelectNode(tn2, false);
					}
				}

				if(bTopSelNode)
				{
					SelectNode(tn, false);
				}

				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion SelectBranch and SelectBranchIterative



		#region DeselectBranch

		/// <summary>
		/// Deselect a branch of TreeNodes.
		/// The TreeNode supplied and the TreeNodes under it, if any (the TreeNodes in its Nodes collection), are deselected.
		/// If bChangeSubBranches is true all the branches of the branches etc of the TreeNode supplied are selected.
		/// </summary>
		/// <param name="tn">TreeNode whose branch (TreeNodes in its Nodes collection) should be deselected.</param>
		/// <param name="bChangeSubBranches">True if all sub-branches should be deselected or false if only one level of child TreeNodes should be deselected.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when deselecting the TreeNodes.</param>
		/// <returns>True if the TreeNode itself and/or its child TreeNodes got selected.</returns>
		public bool DeselectBranch(TreeNode tn, bool bChangeSubBranches, bool bChangeSelNode)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(tn == this.SelNode && bChangeSelNode)
				{
					this.SelNode = null;
				}

				DeselectNode(tn, bChangeSelNode);

				if(tn.Nodes.Count > 0)
				{
					foreach(TreeNode tn2 in tn.Nodes)
					{
						if(bChangeSubBranches)
						{
							DeselectBranch(tn2, bChangeSubBranches, bChangeSelNode);
						}
						else
						{
							DeselectNode(tn2, bChangeSelNode);
						}
					}

				}

				bRetVal = true;
			}

			return bRetVal;
		}

		#endregion DeselectBranch

		#endregion SelectBranch, DeselectBranch



		#region SelectNode, DeselectNode, ToggleNode and ChangeSelNode

		#region SelectNode

		/// <summary>
		/// Select TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to select.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when selecting the TreeNode.</param>
		/// <returns>True if the SelNode property was changed to the TreeNode supplied (even if the SelNode property was already set to the TreeNode supplied before this method was called).</returns>
		public bool SelectNode(TreeNode tn, bool bChangeSelNode)
		{
			return SelectNode(tn, bChangeSelNode, true, true);
		}

		/// <summary>
		/// Select TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to select.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when selecting the TreeNode.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		/// <param name="bTriggerSelNodeEvents">True if calling this method should make the SelNodeInt property raise the OnBeforeSelNodeChanged/OnAfterSelNodeChanged events or false otherwise.</param>
		/// <returns>True if the SelNode property was changed to the TreeNode supplied (even if the SelNode property was already set to the TreeNode supplied before this method was called).</returns>
		protected bool SelectNode(TreeNode tn, bool bChangeSelNode, bool bTriggerSelNodesAddRemoveEvents, bool bTriggerSelNodeEvents)
		{
			bool bRetVal = false;

			if(tn != null)
			{
				if(!this.Enabled)
				{
					//EnableNodes() could be called, but the call to EnableNode(tn) should work.
					//EnableNodes();

					EnableNode(tn);
				}

				#region Switch-statement

				switch(this.MultiSelect)
				{
					case TreeViewMultiSelect.NoMulti:
						if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.Multi:
					case TreeViewMultiSelect.MultiAllDescendants:
						if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.MultiSameBranchAndLevel:
						if(this.SelNodes != null && this.SelNodes.Count > 0)
						{
							TreeNode tnGrandParentSelNodes = null;
							int iLevel = 0;

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								tnGrandParentSelNodes = GetTreeNodeGrandParent(mwtnw.Node);
								iLevel = GetTreeNodeLevel(mwtnw.Node);
								break;
							}

							if(GetTreeNodeGrandParent(tn) == tnGrandParentSelNodes && GetTreeNodeLevel(tn) == iLevel)
							{
								if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
								{
									this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
								}
							}
							else if(IsSelectNodeRegExSatisfied(tn.Text))
							{
								ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

								this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
							}
						}
						else if(IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.MultiSameBranch:
						if(this.SelNodes != null && this.SelNodes.Count > 0)
						{
							TreeNode tnGrandParentSelNodes = null;

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								tnGrandParentSelNodes = GetTreeNodeGrandParent(mwtnw.Node);
								break;
							}

							if(GetTreeNodeGrandParent(tn) == tnGrandParentSelNodes)
							{
								if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
								{
									this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
								}
							}
							else if(IsSelectNodeRegExSatisfied(tn.Text))
							{
								ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

								this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
							}
						}
						else if(IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.MultiSameLevel:
						if(this.SelNodes != null && this.SelNodes.Count > 0)
						{
							int iLevel = 0;

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								iLevel = GetTreeNodeLevel(mwtnw.Node);
								break;
							}

							if(GetTreeNodeLevel(tn) == iLevel)
							{
								if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
								{
									this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
								}
							}
							else if(IsSelectNodeRegExSatisfied(tn.Text))
							{
								ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

								this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
							}
						}
						else if(IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.MultiPathToParents:
					case TreeViewMultiSelect.MultiPathToParent:
						if(this.SelNodes != null && this.SelNodes.Count > 0)
						{
							TreeNode tnGrandParentSelNodes = null;

							foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
							{
								tnGrandParentSelNodes = GetTreeNodeGrandParent(mwtnw.Node);
								break;
							}

							if(GetTreeNodeGrandParent(tn) == tnGrandParentSelNodes || this.MultiSelect == TreeViewMultiSelect.MultiPathToParents)
							{
								if(!IsTreeNodeSelected(tn))
								{
									int iTNLevel = GetTreeNodeLevel(tn);
									TreeNode tnTNGrandParent = GetTreeNodeGrandParent(tn);

									int iMaxLevel = 0;
									foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
									{
										iMaxLevel = Math.Max(iMaxLevel, GetTreeNodeLevel(mwtnw.Node));
									}

									if(iMaxLevel < iTNLevel)
									{
										if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParent)
										{
											ClearSelNodes(bTriggerSelNodesAddRemoveEvents);
										}
										else if(this.MultiSelect == TreeViewMultiSelect.MultiPathToParents)
										{
											DeselectBranch(tnTNGrandParent, true, true);
										}
									}
									else
									{
										foreach(TreeNode tnRoot in this.Nodes)
										{
											if(GetTreeNodeGrandParent(tn) == tnRoot)
											{
												ArrayList al = new ArrayList();

												foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
												{
													if(tnRoot == GetTreeNodeGrandParent(mwtnw.Node))
													{
														int iLevel = GetTreeNodeLevel(mwtnw.Node);

														if(iLevel > iTNLevel)
														{
															al.Add(mwtnw.Node);
														}
													}
												}

												for(int i = 0; i < al.Count; i++)
												{
													DeselectNode(al[i] as TreeNode, false, bTriggerSelNodesAddRemoveEvents);
												}
											}
										}
									}

									if(IsSelectNodeRegExSatisfied(tn.Text))
									{
										this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);

										TreeNode tnTemp = tn;
										while(tnTemp.Parent != null)
										{
											if(!IsTreeNodeSelected(tnTemp.Parent))
											{
												this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
											}

											tnTemp = tnTemp.Parent;
										}
									}
								}
								else
								{
									TreeNode tnTemp = tn;
									while(tnTemp.Parent != null)
									{
										if(IsSelectNodeRegExSatisfied(tn.Text))
										{
											if(!IsTreeNodeSelected(tnTemp.Parent))
											{
												this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
											}
										}

										tnTemp = tnTemp.Parent;
									}
								}
							}
							else if(IsSelectNodeRegExSatisfied(tn.Text))
							{
								ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

								this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);

								TreeNode tnTemp = tn;
								while(tnTemp.Parent != null)
								{
									if(!IsTreeNodeSelected(tnTemp.Parent))
									{
										this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
									}

									tnTemp = tnTemp.Parent;
								}
							}
						}
						else if(IsSelectNodeRegExSatisfied(tn.Text))
						{
							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);

							TreeNode tnTemp = tn;
							while(tnTemp.Parent != null)
							{
								if(!IsTreeNodeSelected(tnTemp.Parent))
								{
									this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
								}

								tnTemp = tnTemp.Parent;
							}
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.SinglePathToParent:
						if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
						{
							ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

							if(this.SelNodes == null)
							{
								this.SelNodes = new Hashtable();
							}

							this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);

							TreeNode tnTemp = tn;
							while(tnTemp.Parent != null)
							{
								if(!IsTreeNodeSelected(tnTemp.Parent))
								{
									this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
								}

								tnTemp = tnTemp.Parent;
							}
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					case TreeViewMultiSelect.SinglePathToParents:
						if(this.SelNodes != null && this.SelNodes.Count > 0)
						{
							int iTNLevel = GetTreeNodeLevel(tn);
							TreeNode tnTNGrandParent = GetTreeNodeGrandParent(tn);

							foreach(TreeNode tnRoot in this.Nodes)
							{
								if(GetTreeNodeGrandParent(tn) == tnRoot)
								{
									if(IsSelectNodeRegExSatisfied(tn.Text))
									{
										ClearSelBranch(tnRoot);

										if(!IsTreeNodeSelected(tn))
										{
											this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
										}

										TreeNode tnTemp = tn;
										while(tnTemp.Parent != null)
										{
											if(!IsTreeNodeSelected(tnTemp.Parent))
											{
												this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
											}

											tnTemp = tnTemp.Parent;
										}
									}

									break;
								}
							}
						}
						else
						{
							if(!IsTreeNodeSelected(tn) && IsSelectNodeRegExSatisfied(tn.Text))
							{
								ClearSelNodes(bTriggerSelNodesAddRemoveEvents);

								if(this.SelNodes == null)
								{
									this.SelNodes = new Hashtable();
								}

								if(!IsTreeNodeSelected(tn))
								{
									this.SelNodesAdd(tn, bTriggerSelNodesAddRemoveEvents);
								}

								TreeNode tnTemp = tn;
								while(tnTemp.Parent != null)
								{
									if(!IsTreeNodeSelected(tnTemp.Parent))
									{
										this.SelNodesAdd(tnTemp.Parent, bTriggerSelNodesAddRemoveEvents);
									}

									tnTemp = tnTemp.Parent;
								}
							}
						}

						if(bChangeSelNode)
						{
							ChangeSelNode(tn, bTriggerSelNodeEvents);
						}
						break;

					default:
						//Execution should never end up here!
						break;
				}

				#endregion Switch-statement

				if(this.SelNode == tn)
				{
					bRetVal = true;
				}


				if(bChangeSelNode && tn != null)
				{
					if(tn.IsExpanded && this.CollapseTreeNodeOnSelect)
					{
						tn.Collapse();
					}
					else if(this.ExpandTreeNodeOnSelect)
					{
						tn.Expand();
					}
				}


				if(!this.Enabled)
				{
					//DisableNodes() could be called, but the call to DisableNode(tn) should work.
					//DisableNodes();

					DisableNode(tn);
				}
				else if(!bActive)
				{
					DeactivateSelNode(tn);
				}
			}

			return bRetVal;
		}

		#endregion SelectNode



		#region DeselectNode

		/// <summary>
		/// Deselect TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to deselect.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when deselecting the TreeNode.</param>
		/// <returns>True if the TreeNode supplied was successfully removed from the SelNodes property.</returns>
		public bool DeselectNode(TreeNode tn, bool bChangeSelNode)
		{
			return DeselectNode(tn, bChangeSelNode, true);
		}

		/// <summary>
		/// Deselect TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to deselect.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when deselecting the TreeNode.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		/// <returns>True if the TreeNode supplied was successfully removed from the SelNodes property.</returns>
		protected bool DeselectNode(TreeNode tn, bool bChangeSelNode, bool bTriggerSelNodesAddRemoveEvents)
		{
			bool bRetVal = false;

			bool bDeselectNode = false;

			if(!this.Enabled)
			{
				//EnableNodes() could be called, but the call to EnableNode(tn) should work.
				//EnableNodes();

				EnableNode(tn);
			}

			if(tn != null && this.SelNodes.Count > 0 && IsTreeNodeSelected(tn))
			{
				if(	this.MultiSelect == TreeViewMultiSelect.MultiPathToParent ||
					this.MultiSelect == TreeViewMultiSelect.MultiPathToParents ||
					this.MultiSelect == TreeViewMultiSelect.SinglePathToParent ||
					this.MultiSelect == TreeViewMultiSelect.SinglePathToParents)
				{
					if(tn.Nodes.Count == 0 || !IsAnyChildTreeNodeSelected(tn))
					{
						bDeselectNode = true;
					}
				}
				else
				{
					bDeselectNode = true;
				}
			}

			if(!this.AllowNoSelNode && this.SelNodes.Count == 1)
			{
				bDeselectNode = false;
			}

			if(bDeselectNode)
			{
				this.SelNodesRemove(tn, bTriggerSelNodesAddRemoveEvents);

				if(!IsTreeNodeSelected(tn))
				{
					bRetVal = true;
				}
			}

			if(bChangeSelNode)
			{
				if(bChangeSelNode && tn != null)
				{
					if(tn.IsExpanded && this.CollapseTreeNodeOnDeselect)
					{
						tn.Collapse();
					}
					else if(this.ExpandTreeNodeOnDeselect)
					{
						tn.Expand();
					}
				}

				if(this.SelNode != null)
				{
					bRetVal = true;
				}

				this.SelNodeInt = null;
			}

			if(!this.Enabled)
			{
				//DisableNodes() could be called, but the call to DisableNode(tn) should work.
				//DisableNodes();

				DisableNode(tn);
			}

			return bRetVal;
		}

		#endregion DeselectNode



		#region ToggleNode

		/// <summary>
		/// Toggle the selection of TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to toggle selection of.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when toggling the selection of the TreeNode.</param>
		/// <returns>True if supplied TreeNode got selected or false otherwise.</returns>
		public bool ToggleNode(TreeNode tn, bool bChangeSelNode)
		{
			return ToggleNode(tn, bChangeSelNode, true);
		}

		/// <summary>
		/// Toggle the selection of TreeNode supplied.
		/// Note that all selected TreeNodes can be found in the SelNodes property and the most recently selected TreeNode can be found in the SelNode property.
		/// </summary>
		/// <param name="tn">TreeNode to toggle selection of.</param>
		/// <param name="bChangeSelNode">True if the SelNode property should be changed when toggling the selection of the TreeNode.</param>
		/// <param name="bTriggerSelNodesAddRemoveEvents">True if calling this method should make the SelNodesAdd/SelNodesRemove methods raise the OnBeforeSelNodesAdd/OnBeforeSelNodesRemove and OnAfterSelNodesAdd/OnAfterSelNodesRemove events or false otherwise.</param>
		/// <returns>True if supplied TreeNode got selected or false otherwise.</returns>
		protected bool ToggleNode(TreeNode tn, bool bChangeSelNode, bool bTriggerSelNodesAddRemoveEvents)
		{
			bool bRetVal = true;

			if(this.SelNodes != null && IsTreeNodeSelected(tn))
			{
				DeselectNode(tn, bChangeSelNode);
			}
			else
			{
				SelectNode(tn, bChangeSelNode);

				bRetVal = false;
			}

			return bRetVal;
		}

		#endregion ToggleNode



		#region ChangeSelNode

		/// <summary>
		/// Change SelNode to the TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to change SelNode into.</param>
		/// <param name="bTriggerSelNodeEvents">True if calling this method should make the SelNodeInt property raise the OnBeforeSelNodeChanged/OnAfterSelNodeChanged events or false otherwise.</param>
		protected void ChangeSelNode(TreeNode tn, bool bTriggerSelNodeEvents)
		{
			if(bTriggerSelNodeEvents)
			{
				this.SelNodeInt = tn;
			}
			else
			{
				this.SelNodeIntSilent = tn;
			}

			if(this.ScrollToSelNode && !bRubberbandHasBeenPainted)
			{
				if(tn != null)
				{
					tn.EnsureVisible();
				}
			}
		}

		#endregion ChangeSelNode

		#endregion SelectNode, DeselectNode, ToggleNode and ChangeSelNode

		#endregion Modification Public Help Methods

		#endregion Public Help Methods



		#region Protected Help Methods

		#region Modification Protected Help Methods

		#region EnsureAllCheckedNodesAreAllowed

		/// <summary>
		/// Makes sure all the selected TreeNodes are allowed according to the SelectNodeRegEx property's regular expression.
		/// </summary>
		protected void EnsureAllCheckedNodesAreAllowed()
		{
			ArrayList al = new ArrayList();

			if(this.CheckedNodes != null)
			{
				foreach(TreeNode tn in this.CheckedNodes.Values)
				{
					if(!IsCheckNodeRegExSatisfied(tn.Text))
					{
						al.Add(tn);
					}
				}
			}

			bForceCheckNode = true;

			for(int i = 0; i < al.Count; i++)
			{
				UncheckNode(al[i] as TreeNode, true);
			}

			bForceCheckNode = false;
		}

		#endregion EnsureAllCheckedNodesAreAllowed



		#region EnsureAllSelectedNodesAreAllowed

		/// <summary>
		/// Makes sure all the selected TreeNodes are allowed according to the SelectNodeRegEx property's regular expression.
		/// </summary>
		protected void EnsureAllSelectedNodesAreAllowed()
		{
			ArrayList al = new ArrayList();

			if(this.SelNodes != null)
			{
				foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
				{
					if(!IsSelectNodeRegExSatisfied(mwtnw.Node.Text))
					{
						al.Add(mwtnw.Node);
					}
				}
			}

			for(int i = 0; i < al.Count; i++)
			{
				DeselectNode(al[i] as TreeNode, false);
			}

			if(this.SelNode != null && !IsSelectNodeRegExSatisfied(this.SelNode.Text))
			{
				DeselectNode(this.SelNode, true);
			}
		}

		#endregion EnsureAllSelectedNodesAreAllowed

		#endregion Modification Protected Help Methods

		#endregion Protected Help Methods



		#region Private Help Methods

		#region Retrieval Private Help Methods

		#region AllowMouseClearSelNodes

		/// <summary>
		/// Check if the mouse click should clear all selected TreeNodes depending on the MouseClearSelNodes and FullRowSelect properties and properties passed in.
		/// Note that this method does not take into consideration which mouse button is clicked or which control keys are pressed.
		/// </summary>
		/// <param name="tn">TreeNode that was clicked.</param>
		/// <param name="pt">Point (client coordinates) that was clicked.</param>
		/// <returns>True if the mouse click should clear all selected TreeNodes or false otherwise.</returns>
		private bool AllowMouseClearSelNodes(TreeNode tn, Point pt)
		{
			bool bRet = false;

			switch(this.MouseClearSelNodes)
			{
				case MouseClearSelNodes.Bottom:
					if(this.FullRowSelect)
					{
						bRet = tnFullRowSelect == null;
					}
					else
					{
						bRet = tn == null;
					}
					break;

				case MouseClearSelNodes.LeftSide:
					if(!this.FullRowSelect)
					{
						bRet = !bExpandInProgress && !bCollapseInProgress && tn != null && !IsPointRightOfTreeNode(tn, pt);
					}
					break;

				case MouseClearSelNodes.RightSide:
					if(!this.FullRowSelect)
					{
						bRet = tn != null && !IsPointLeftOfTreeNode(tn, pt);
					}
					break;

				case MouseClearSelNodes.BothSides:
					if(!this.FullRowSelect)
					{
						bRet = !bExpandInProgress && !bCollapseInProgress && tn != null && !IsPointOnTreeNode(tn, pt);
					}
					break;

				case MouseClearSelNodes.BottomAndLeftSide:
					if(this.FullRowSelect)
					{
						bRet = tnFullRowSelect == null;
					}
					else
					{
						bRet = tn == null || !bExpandInProgress && !bCollapseInProgress && !IsPointRightOfTreeNode(tn, pt);
					}
					break;

				case MouseClearSelNodes.BottomAndRightSide:
					if(this.FullRowSelect)
					{
						bRet = tnFullRowSelect == null;
					}
					else
					{
						bRet = tn == null || !IsPointLeftOfTreeNode(tn, pt);
					}
					break;

				case MouseClearSelNodes.BottomAndBothSides:
					if(this.FullRowSelect)
					{
						bRet = tnFullRowSelect == null;
					}
					else
					{
						bRet = tn == null || !bExpandInProgress && !bCollapseInProgress && !IsPointOnTreeNode(tn, pt);
					}
					break;

				default:
					//MouseClearSelNodes.None ends up here, returning false.
					break;
			}

			return bRet;
		}

		#endregion AllowMouseClearSelNodes



		#region IsMouseMoveSelect

		/// <summary>
		/// Returns true if a mouse move is considered to be a select operation or false otherwise.
		/// This property is used for rubberband selection and a TreeNode is expanded so that the TreeView is scrolled - no selection
		///		of TreeNodes will occur for this until the mouse is moved slightly. Without this property TreeNodes were selected when they
		///		shouldn't have been.
		///	Note that the distance the mouse has to be moved to start selecting TreeNodes after a TreeNode has been expanded is the same as the
		///		distance in which two consecutive Mouse Clicks are considered to be one DoubleClick (not talking about the time constraint
		///		here).
		///	Note that all this only applies when the left mouse button is still held down, if it is released all this does not apply.
		/// </summary>
		private bool IsMouseMoveSelect
		{
			get
			{
				Point ptMouseNow = this.PointToClient(Control.MousePosition);

				return Math.Abs(ptMouseDownOrig.X - ptMouseNow.X) > SystemInformation.DoubleClickSize.Width || Math.Abs(ptMouseDownOrig.Y - ptMouseNow.Y) > SystemInformation.DoubleClickSize.Height;
			}
		}

		#endregion IsMouseMoveSelect



		#region KeyboardModKeyNavigation

		/// <summary>
		/// Return true if the MWTreeView navigation/scrolling should be activated or false otherwise.
		/// Note that pressing the Control-Key enables MWTreeView navigation/scrolling.
		/// </summary>
		private bool KeyboardModKeyNavigation(Keys k)
		{
			return
				this.AllowControlKeyNavigation &&
				(k & Keys.Control) == Keys.Control;
		}

		#endregion KeyboardModKeyNavigation



		#region Keyboard ModifierKeys Pressed State (MouseModKeyAlt, MouseModKeyControl & MouseModKeyShift)

		/// <summary>
		/// Get the pressed state of the Alt-Key.
		/// Returns true if the Alt-Key is pressed or false otherwise.
		/// </summary>
		private bool KeyboardModKeyAlt(Keys k)
		{
			return
				this.AllowKeyboardModKeyAlt &&
				(k & Keys.Alt) == Keys.Alt;
		}

		/// <summary>
		/// Get the pressed state of the Control-Key.
		/// Returns true if the Control-Key is pressed or false otherwise.
		/// </summary>
		private bool KeyboardModKeyControl(Keys k)
		{
			return
				this.AllowKeyboardModKeyControl &&
				(k & Keys.Control) == Keys.Control;
		}

		/// <summary>
		/// Get the pressed state of the Shift-Key.
		/// Returns true if the Shift-Key is pressed or false otherwise.
		/// </summary>
		private bool KeyboardModKeyShift(Keys k)
		{
			return
				this.AllowKeyboardModKeyShift &&
				(k & Keys.Shift) == Keys.Shift;
		}

		#endregion Keyboard ModifierKeys Pressed State (MouseModKeyAlt, MouseModKeyControl & MouseModKeyShift)



		#region Mouse ModifierKeys Pressed State (MouseModKeyAlt, MouseModKeyControl & MouseModKeyShift)

		/// <summary>
		/// Get the pressed state of the Alt-Key.
		/// Returns true if the Alt-Key is pressed or false otherwise.
		/// </summary>
		private bool MouseModKeyAlt
		{
			get
			{
				return
					this.AllowMouseModKeyAlt &&
					(Control.ModifierKeys & Keys.Alt) == Keys.Alt;
			}
		}

		/// <summary>
		/// Get the pressed state of the Control-Key.
		/// Returns true if the Control-Key is pressed or false otherwise.
		/// </summary>
		private bool MouseModKeyControl
		{
			get
			{
				return
					this.AllowMouseModKeyControl &&
					(Control.ModifierKeys & Keys.Control) == Keys.Control;
			}
		}

		/// <summary>
		/// Get the pressed state of the Shift-Key.
		/// Returns true if the Shift-Key is pressed or false otherwise.
		/// </summary>
		private bool MouseModKeyShift
		{
			get
			{
				return
					this.AllowMouseModKeyShift &&
					(Control.ModifierKeys & Keys.Shift) == Keys.Shift;
			}
		}

		#endregion Mouse ModifierKeys Pressed State (MouseModKeyAlt, MouseModKeyControl & MouseModKeyShift)

		#endregion Retrieval Private Help Methods



		#region Modification Private Help Methods

		#region ActivateSelNodes, DeactivateSelNodes and ForceDeactivateSelNodes

		#region ActivateSelNodes

		/// <summary>
		/// Activate all selected TreeNodes of this MWTreeView.
		/// Activating means higlighting.
		/// </summary>
		private void ActivateSelNodes()
		{
			if(this.SelNodes != null && this.SelNodes.Count > 0)
			{
				bActive = true;

				foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
				{
					HighlightNode(mwtnw.Node);
				}
			}
		}

		#endregion ActivateSelNodes



		#region DeactivateSelNode (2 overrides)

		/// <summary>
		/// Deactivate TreeNode supplied.
		/// Deactivating means removing higlighting ('lowlighting').
		/// </summary>
		/// <param name="tn">TreeNode that should be deactivated.</param>
		private void DeactivateSelNode(TreeNode tn)
		{
			if(tn != null)
			{
				MWTreeNodeWrapper mwtnw = this.SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

				if(mwtnw != null)
				{
					DeactivateSelNode(mwtnw);
				}
			}
		}

		/// <summary>
		/// Deactivate TreeNode of MWTreeNodeWrapper supplied.
		/// Deactivating means removing higlighting ('lowlighting').
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper whose TreeNode should be deactivated.</param>
		private void DeactivateSelNode(MWTreeNodeWrapper mwtnw)
		{
			if(mwtnw != null && mwtnw.Node != null)
			{
				if(this.HideSelection)
				{
					if(IsPlainColoredTreeNode(mwtnw.Node))
					{
						LowlightNode(mwtnw.Node);
					}
					else
					{
						mwtnw.Node.BackColor = mwtnw.BackColor;
						mwtnw.Node.ForeColor = mwtnw.ForeColor;
					}
				}
				else
				{
					HighlightNode(mwtnw.Node, false);
				}
			}
		}

		#endregion DeactivateSelNode (2 overrides)



		#region DeactivateSelNodes

		/// <summary>
		/// Deactivate all selected TreeNodes of this MWTreeView.
		/// Deactivating means removing higlighting ('lowlighting').
		/// </summary>
		private void DeactivateSelNodes()
		{
			if(this.SelNodes != null && this.SelNodes.Count > 0)
			{
				bActive = false;

				if(this.HideSelection)
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						if(IsPlainColoredTreeNode(mwtnw.Node))
						{
							LowlightNode(mwtnw.Node);
						}
						else
						{
							if(mwtnw.Node.BackColor != mwtnw.BackColor)
							{
								mwtnw.Node.BackColor = mwtnw.BackColor;
							}

							if(mwtnw.Node.ForeColor != mwtnw.ForeColor)
							{
								mwtnw.Node.ForeColor = mwtnw.ForeColor;
							}

						}
					}
				}
				else
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						HighlightNode(mwtnw.Node, false);
					}
				}
			}
		}

		#endregion DeactivateSelNodes



		#region ForceDeactivateSelNodes

		/// <summary>
		/// Forces deactivation of all selected TreeNodes of this MWTreeView.
		/// Deactivating means removing higlighting ('lowlighting').
		/// </summary>
		private void ForceDeactivateSelNodes()
		{
			if(this.SelNodes != null && this.SelNodes.Count > 0)
			{
				foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
				{
					if(IsPlainColoredTreeNode(mwtnw.Node))
					{
						LowlightNode(mwtnw.Node);
					}
					else
					{
						mwtnw.Node.BackColor = mwtnw.BackColor;
						mwtnw.Node.ForeColor = mwtnw.ForeColor;
					}
				}
			}
		}

		#endregion ForceDeactivateSelNodes

		#endregion ActivateSelNodes, DeactivateSelNodes and ForceDeactivateSelNodes



		#region ChangeSelNodesColors

		/// <summary>
		/// Change the colors of the SelNodes TreeNodes.
		/// </summary>
		private void ChangeSelNodesColors()
		{
			if(this.Enabled && this.SelNodes != null)
			{
				if(this.UseExtendedSelectionColors)
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						mwtnw.Node.BackColor = this.SelectedTreeNodeBackColor;
						mwtnw.Node.ForeColor = this.SelectedTreeNodeForeColor;
					}
				}
				else
				{
					foreach(MWTreeNodeWrapper mwtnw in this.SelNodes.Values)
					{
						mwtnw.Node.BackColor = SystemColors.Highlight;
						mwtnw.Node.ForeColor = SystemColors.HighlightText;
					}
				}

				if(!bActive)
				{
					DeactivateSelNodes();
				}
			}
		}

		#endregion ChangeSelNodesColors



		#region DisableNode(s), EnableNode(s)

		#region DisableNode

		/// <summary>
		/// Disable TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to disable.</param>
		private void DisableNode(TreeNode tn)
		{
			if(this.UseExtendedDisabledColors)
			{
				htDisabledTreeNodes.Add(tn.GetHashCode(), new MWTreeNodeWrapper(tn, this.DisabledTreeNodeBackColor, this.DisabledTreeNodeForeColor));
			}
			else
			{
				if(IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = GetSelectedMWTreeNodeWrapper(tn);

					if(mwtnw != null)
					{
						if(this.HideSelection)
						{
							htDisabledTreeNodes.Add(tn.GetHashCode(), new MWTreeNodeWrapper(tn, mwtnw.BackColor, mwtnw.ForeColor));
						}
						else
						{
							htDisabledTreeNodes.Add(tn.GetHashCode(), new MWTreeNodeWrapper(tn, SystemColors.Control, SystemColors.ControlDark));
						}
					}
				}
				else if(IsPlainColoredTreeNode(tn))
				{
					htDisabledTreeNodes.Add(tn.GetHashCode(), new MWTreeNodeWrapper(tn, this.DisabledTreeNodeBackColor, this.DisabledTreeNodeForeColor));
				}
			}
		}

		#endregion DisableNode



		#region DisableNodes

		/// <summary>
		/// Disable TreeNodes.
		/// </summary>
		private void DisableNodes()
		{
			this.SelectedNode = null;

			TreeNode tn = this.TopNode;

			htDisabledTreeNodes = new Hashtable();

			while(tn != null)
			{
				DisableNode(tn);

				if(tn != tn.NextVisibleNode)
				{
					tn = tn.NextVisibleNode;
				}
				else
				{
					break;
				}
			}
		}

		#endregion DisableNodes



		#region EnableNode

		/// <summary>
		/// Enable TreeNode supplied.
		/// </summary>
		/// <param name="tn">TreeNode to enable.</param>
		private void EnableNode(TreeNode tn)
		{
			MWTreeNodeWrapper mwtnw = GetDisabledMWTreeNodeWrapper(tn);

			if(mwtnw != null)
			{
				EnableNode(mwtnw);

				htDisabledTreeNodes.Remove(tn.GetHashCode());
			}
		}

		/// <summary>
		/// Enable MWTreeNodeWrapper supplied.
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper to enable.</param>
		private void EnableNode(MWTreeNodeWrapper mwtnw)
		{
			if(IsTreeNodeSelected(mwtnw.Node))
			{
				if(this.HideSelection && !this.Focused || !this.Enabled)
				{
					(GetSelectedMWTreeNodeWrapper(mwtnw.Node)).Enable();
				}
				else
				{
					if(this.UseExtendedDisabledColors)
					{
						mwtnw.Enable();
					}
					else
					{
						mwtnw.Node.BackColor = SystemColors.Control;
						mwtnw.Node.ForeColor = SystemColors.WindowText;
					}
				}
			}
			else
			{
				mwtnw.Enable();
			}
		}

		#endregion EnableNode



		#region EnableNodes

		/// <summary>
		/// Enable TreeNodes.
		/// </summary>
		private void EnableNodes()
		{
			TreeNode tn = this.TopNode;

			if(htDisabledTreeNodes != null && htDisabledTreeNodes.Values.Count > 0)
			{
				foreach(MWTreeNodeWrapper mwtnw in htDisabledTreeNodes.Values)
				{
					EnableNode(mwtnw);
				}

				htDisabledTreeNodes.Clear();
			}

			this.SelectedNode = this.SelNode;
		}

		#endregion EnableNodes

		#endregion DisableNode(s), EnableNode(s)

		#endregion Modification Private Help Methods

		#endregion Private Help Methods



		#region Public Static Help Methods

		#region Retrieval Public Static Help Methods

		#region CountTreeNodes

		/// <summary>
		/// Count the number of TreeNodes in a TreeView.
		/// </summary>
		/// <param name="tv">TreeView to count the number of TreeNodes in.</param>
		/// <returns>The number of TreeNodes in a TreeView.</returns>
		public static int CountTreeNodes(TreeView tv)
		{
			int i = 0;

			foreach(TreeNode tn in tv.Nodes)
			{
				if(tn.Nodes.Count > 0)
				{
					i += CountTreeNodes(tn);
				}
				else
				{
					i++;
				}
			}

			return i;
		}

		/// <summary>
		/// Count the number of TreeNodes that reside under the TreeNode supplied.
		/// Note that the TreeNode supplied is also included in the count.
		/// </summary>
		/// <param name="tn">TreeNode to count the number of TreeNodes in.</param>
		/// <returns>The number of TreeNodes in a TreeNode including the TreeNode supplied.</returns>
		public static int CountTreeNodes(TreeNode tn)
		{
			int i = 0;

			foreach(TreeNode tn2 in tn.Nodes)
			{
				if(tn2.Nodes.Count > 0)
				{
					i += CountTreeNodes(tn2);
				}
				else
				{
					i++;
				}
			}

			return ++i;
		}

		/// <summary>
		/// Count the number of TreeNodes in the TreeNode supplied and all its sibling TreeNodes if the bSiblingTreeNodes parameter is true.
		/// Note that the TreeNode supplied is also included in the count.
		/// Note that the parent TreeNode of the TreeNode supplied is not included in the count.
		/// </summary>
		/// <param name="tn">TreeNode to count the number of TreeNodes in.</param>
		/// <param name="bSiblingTreeNodes">True if the sibling TreeNodes of the TreeNode supplied should be included in the count.</param>
		/// <returns>The number of TreeNodes in a TreeNode including the TreeNode supplied.</returns>
		public static int CountTreeNodes(TreeNode tn, bool bSiblingTreeNodes)
		{
			int i = 0;

			if(bSiblingTreeNodes)
			{
				if(tn.Parent == null)
				{
					i = CountTreeNodes(tn.TreeView);
				}
				else
				{
					foreach(TreeNode tn2 in tn.Parent.Nodes)
					{
						i += CountTreeNodes(tn2);
					}
				}
			}
			else
			{
				i = CountTreeNodes(tn);
			}

			return i;
		}

		#endregion CountTreeNodes



		#region GetBackColor, GetForeColor

		#region GetBackColor

		/// <summary>
		/// Get the BackColor of the TreeNode supplied.
		/// Note that this method works even if the TreeNode is selected and/or if the MWTreeView is disabled (Enabled property set to false).
		/// </summary>
		/// <param name="tn">TreeNode to get the BackColor of.</param>
		/// <returns>BackColor of the TreeNode supplied.</returns>
		public static Color GetBackColor(TreeNode tn)
		{
			Color col = tn.TreeView.BackColor;

			if((tn.TreeView as MWTreeView).IsTreeNodeSelected(tn))
			{
				col = (tn.TreeView as MWTreeView).GetSelectedMWTreeNodeWrapper(tn).BackColor;
			}
			else if((tn.TreeView as MWTreeView).IsTreeNodeDisabled(tn))
			{
				col = (tn.TreeView as MWTreeView).GetDisabledMWTreeNodeWrapper(tn).BackColor;
			}
			else
			{
				col = tn.BackColor;
			}

			return col;
		}

		/// <summary>
		/// Get the BackColor of the MWTreeNodeWrapper supplied.
		/// Note that this method works even if the MWTreeNodeWrapper is selected and/or if the MWTreeView is disabled (Enabled property set to false).
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper to get the BackColor of.</param>
		/// <returns>BackColor of the MWTreeNodeWrapper supplied.</returns>
		public static Color GetBackColor(MWTreeNodeWrapper mwtnw)
		{
			return GetBackColor(mwtnw.Node);
		}

		#endregion GetBackColor



		#region GetForeColor

		/// <summary>
		/// Get the ForeColor of the TreeNode supplied.
		/// Note that this method works even if the TreeNode is selected and/or if the MWTreeView is disabled (Enabled property set to false).
		/// </summary>
		/// <param name="tn">TreeNode to get the ForeColor of.</param>
		/// <returns>ForeColor of the TreeNode supplied.</returns>
		public static Color GetForeColor(TreeNode tn)
		{
			Color col = tn.TreeView.ForeColor;

			if((tn.TreeView as MWTreeView).IsTreeNodeSelected(tn))
			{
				col = (tn.TreeView as MWTreeView).GetSelectedMWTreeNodeWrapper(tn).ForeColor;
			}
			else if((tn.TreeView as MWTreeView).IsTreeNodeDisabled(tn))
			{
				col = (tn.TreeView as MWTreeView).GetDisabledMWTreeNodeWrapper(tn).ForeColor;
			}
			else
			{
				col = tn.ForeColor;
			}

			return col;
		}

		/// <summary>
		/// Get the ForeColor of the MWTreeNodeWrapper supplied.
		/// Note that this method works even if the MWTreeNodeWrapper is selected and/or if the MWTreeView is disabled (Enabled property set to false).
		/// </summary>
		/// <param name="mwtnw">MWTreeNodeWrapper to get the ForeColor of.</param>
		/// <returns>ForeColor of the MWTreeNodeWrapper supplied.</returns>
		public static Color GetForeColor(MWTreeNodeWrapper mwtnw)
		{
			return GetForeColor(mwtnw.Node);
		}

		#endregion GetForeColor

		#endregion GetBackColor, GetForeColor



		#region GetTreeNodeGrandParent

		/// <summary>
		/// Get the GrandParent of the TreeNode supplied.
		/// The GrandParent is the outermost Parent of a TreeNode.
		/// </summary>
		/// <param name="tn">TreeNode to get GrandParent of.</param>
		/// <returns>The GrandParent of the TreeNode supplied.</returns>
		public static TreeNode GetTreeNodeGrandParent(TreeNode tn)
		{
			if(tn != null && tn.Parent != null)
			{
				return GetTreeNodeGrandParent(tn.Parent);
			}
			else
			{
				return tn;
			}
		}

		#endregion GetTreeNodeGrandParent



		#region GetTreeNodeLevel

		/// <summary>
		/// Get the TreeNodeLevel of the TreeNode supplied.
		/// Note that the TreeNodes in the TreeView's Nodes collection are considered to be of TreeNodeLevel zero (0) and each consecutive level thereafter is one more (1, 2, etc).
		/// </summary>
		/// <param name="tn">TreeNode whose TreeNodeLevel should be checked.</param>
		/// <returns>The TreeNodeLevel of the TreeNode supplied.</returns>
		public static int GetTreeNodeLevel(TreeNode tn)
		{
			int i = 0;

			if(tn != null && tn.Parent != null)
			{
				return GetTreeNodeLevel(tn.Parent) + 1;
			}
			else
			{
				return i;
			}
		}

		#endregion GetTreeNodeLevel



		#region IsPointOnTreeNode and similar methods

		#region IsPointOnTreeNode

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the TreeNode supplied.
		/// Note that being on a TreeNode is considered to be when the Point is over the text, image if there is one or CheckBox if there is one.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on it.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNode(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				int iWidthDifference = 0;

				if(tn.TreeView.ImageList != null)
				{
					//The figure 3 is the space between the image and the text.
					iWidthDifference += tn.TreeView.ImageList.ImageSize.Width + 3;
				}

				if(tn.TreeView.CheckBoxes)
				{
					//The figure 3 is comprised of the space to the right of the CheckBox (1) and the space to the left of the CheckBox (2).
					//Hope that a normal CheckBox is the same size as a 'menu check mark'...
					iWidthDifference += SystemInformation.MenuCheckSize.Width + 3;
				}

				//The figure 1 is some padding to the right of the text.
				Rectangle rct = new Rectangle(
					tn.Bounds.X - iWidthDifference,
					tn.Bounds.Y,
					tn.Bounds.Width + iWidthDifference + 1,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNode



		#region IsPointOnTreeNodeText and IsPointOnTreeNodeTextExact

		#region IsPointOnTreeNodeText

		/// <summary>
		/// Check if the Point (client coordinates) supplied is in the rectangle for the text of the TreeNode supplied.
		/// This method gives the result that the original TreeView uses.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is in the rectangle for the text.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is in the rectangle of the text of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is in the rectangle of the text of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeText(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				//The figure 1 is some padding to the right of the text.
				Rectangle rct = new Rectangle(
					tn.Bounds.X,
					tn.Bounds.Y,
					tn.Bounds.Width + 1,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeText



		#region IsPointOnTreeNodeTextExact

		/// <summary>
		/// Check if the Point (client coordinates) supplied is in the rectangle for the text of the TreeNode supplied.
		/// This method gives an exact result from a visual perspective.
		/// Note that the underlying TreeView paints the BackColor and focus rectangle a bit imprecise. There are inconsistencies when the TreeView
		///		has  NO(!) horizontal scrollbar visible. When it has a horizontal scrollbar it is spot on though.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is in the rectangle for the text.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is in the rectangle of the text of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is in the rectangle of the text of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeTextExact(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				bRet = tn.Bounds.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeTextExact

		#endregion IsPointOnTreeNodeText and IsPointOnTreeNodeTextExact



		#region IsPointOnTreeNodeRow

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the row of the TreeNode supplied.
		/// Note that being on the row of a TreeNode is considered to be when the Point is on the TreeNode vertically only.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on its row.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the row of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the row of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeRow(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				bRet = pt.Y >= tn.Bounds.Y && pt.Y <= tn.Bounds.Y + tn.Bounds.Height;
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeRow



		#region IsPointOnTreeNodeImage and IsPointOnTreeNodeImageExact

		#region IsPointOnTreeNodeImage

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the Image of the TreeNode supplied.
		/// This method gives the result that the original TreeView uses.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on the Image.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the Image of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the Image of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeImage(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null && tn.TreeView.ImageList != null && tn.TreeView.ImageList.Images.Count > 0)
			{
				Rectangle rct = new Rectangle(
					tn.Bounds.X - tn.TreeView.ImageList.ImageSize.Width - 3,
					tn.Bounds.Y,
					tn.TreeView.ImageList.ImageSize.Width + 3,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeImage



		#region IsPointOnTreeNodeImageExact

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the Image of the TreeNode supplied.
		/// This method gives an exact result from a visual perspective.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on the Image.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the Image of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the Image of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeImageExact(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null && tn.TreeView.ImageList != null && tn.TreeView.ImageList.Images.Count > 0)
			{
				Rectangle rct = new Rectangle(
					tn.Bounds.X - tn.TreeView.ImageList.ImageSize.Width - 3,
					tn.Bounds.Y + (tn.Bounds.Height - tn.TreeView.ImageList.ImageSize.Height) / 2,
					tn.TreeView.ImageList.ImageSize.Width,
					tn.TreeView.ImageList.ImageSize.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeImageExact

		#endregion IsPointOnTreeNodeImage and IsPointOnTreeNodeImageExact



		#region IsPointOnTreeNodeCheckBox and IsPointOnTreeNodeCheckBoxExact

		#region IsPointOnTreeNodeCheckBox

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the CheckBox of the TreeNode supplied.
		/// This method gives the result that the original TreeView uses.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on the CheckBox.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the CheckBox of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the CheckBox of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeCheckBox(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null && tn.TreeView.CheckBoxes)
			{
				int iWidthDifference = 0;

				if(tn.TreeView.ImageList != null)
				{
					//The figure 3 is the space between the image and the text.
					iWidthDifference += tn.TreeView.ImageList.ImageSize.Width + 3;
				}

				//The figure 3 is comprised of the space to the right of the CheckBox (1) and the space to the left of the CheckBox (2).
				Rectangle rct = new Rectangle(
					tn.Bounds.X - iWidthDifference - SystemInformation.MenuCheckSize.Width - 3,
					tn.Bounds.Y,
					SystemInformation.MenuCheckSize.Width + 3,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeCheckBox



		#region IsPointOnTreeNodeCheckBoxExact

		/// <summary>
		/// Check if the Point (client coordinates) supplied is on the CheckBox of the TreeNode supplied.
		/// This method gives an exact result from a visual perspective.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is on the CheckBox.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is on the CheckBox of the TreeNode supplied.</param>
		/// <returns>True if Point supplied is on the CheckBox of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointOnTreeNodeCheckBoxExact(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null && tn.TreeView.CheckBoxes)
			{
				int iWidthDifference = 0;

				if(tn.TreeView.ImageList != null)
				{
					//The figure 3 is the space between the image and the text.
					iWidthDifference += tn.TreeView.ImageList.ImageSize.Width + 3;
				}

				//The figure 1 is the space to the right of the CheckBox.
				Rectangle rct = new Rectangle(
					tn.Bounds.X - iWidthDifference - SystemInformation.MenuCheckSize.Width - 1,
					tn.Bounds.Bottom - SystemInformation.MenuCheckSize.Height - 1,
					SystemInformation.MenuCheckSize.Width,
					SystemInformation.MenuCheckSize.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointOnTreeNodeCheckBoxExact

		#endregion IsPointOnTreeNodeCheckBox and IsPointOnTreeNodeCheckBoxExact



		#region IsPointLeftOfTreeNode

		/// <summary>
		/// Check if the Point (client coordinates) supplied is to the left of the TreeNode supplied.
		/// Note that being to the left of a TreeNode is considered to be when the Point is to the left of the text, image if there is one and/or CheckBox if there is one.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is to the left of it.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is to the left the TreeNode supplied.</param>
		/// <returns>True if Point supplied is to the left of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointLeftOfTreeNode(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				int iWidthDifference = 0;

				if(tn.TreeView.ImageList != null)
				{
					//The figure 3 is the space between the image and the text.
					iWidthDifference += tn.TreeView.ImageList.ImageSize.Width + 3;
				}

				if(tn.TreeView.CheckBoxes)
				{
					//The figure 3 is comprised of the space to the right of the CheckBox (1) and the space to the left of the CheckBox (2).
					//Hope that a normal CheckBox is the same size as a 'menu check mark'...
					iWidthDifference += SystemInformation.MenuCheckSize.Width + 3;
				}

				//The figure 1 is some padding to the right of the text.
				Rectangle rct = new Rectangle(
					0,
					tn.Bounds.Y,
					tn.Bounds.X - iWidthDifference,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointLeftOfTreeNode



		#region IsPointRightOfTreeNode

		/// <summary>
		/// Check if the Point (client coordinates) supplied is to the right of the TreeNode supplied.
		/// Note that being to the right of a TreeNode is considered to be when the Point is to the right of the text.
		/// </summary>
		/// <param name="tn">TreeNode that should be checked if the Point supplied is to the right of it.</param>
		/// <param name="pt">Point (client coordinates) that should be checked if it is to the right the TreeNode supplied.</param>
		/// <returns>True if Point supplied is to the right of the TreeNode supplied or false otherwise.</returns>
		public static bool IsPointRightOfTreeNode(TreeNode tn, Point pt)
		{
			bool bRet = false;

			if(tn != null)
			{
				int iWidthDifference = 0;

				if(tn.TreeView.ImageList != null)
				{
					//The figure 3 is the space between the image and the text.
					iWidthDifference += tn.TreeView.ImageList.ImageSize.Width + 3;
				}

				if(tn.TreeView.CheckBoxes)
				{
					//The figure 3 is comprised of the space to the right of the CheckBox (1) and the space to the left of the CheckBox (2).
					//Hope that a normal CheckBox is the same size as a 'menu check mark'...
					iWidthDifference += SystemInformation.MenuCheckSize.Width + 3;
				}

				//The figure 1 is some padding to the right of the text.
				Rectangle rct = new Rectangle(
					tn.Bounds.Right + 1,
					tn.Bounds.Y,
					tn.TreeView.ClientRectangle.Width - tn.Bounds.Right - 1,
					tn.Bounds.Height);

				bRet = rct.Contains(pt);
			}

			return bRet;
		}

		#endregion IsPointRightOfTreeNode

		#endregion IsPointOnTreeNode and similar methods

		#endregion Retrieval Public Static Help Methods



		#region Modification Public Static Help Methods

		#region ChangeTreeNodeImageIndex, ChangeTreeNodeImageIndices and ChangeTreeNodeSelectedImageIndex

		#region ChangeTreeNodeImageIndex

		/// <summary>
		/// Change the ImageIndex of the supplied TreeNode.
		/// </summary>
		/// <param name="tn">TreeNode to change the ImageIndex of.</param>
		/// <param name="iImageIndex">ImageIndex to change to.</param>
		public static void ChangeTreeNodeImageIndex(TreeNode tn, int iImageIndex)
		{
			if(tn != null)
			{
				if((tn.TreeView as MWTreeView).IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = (tn.TreeView as MWTreeView).SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					if(mwtnw != null)
					{
						mwtnw.ImageIndex = iImageIndex;
					}
				}
				else
				{
					tn.ImageIndex = iImageIndex;
				}
			}
		}

		/// <summary>
		/// Change the ImageIndex of the supplied MWTreeNodeWrapper's TreeNode.
		/// </summary>
		/// <param name="tn">MWTreeNodeWrapper whose TreeNode to change the ImageIndex of.</param>
		/// <param name="iImageIndex">ImageIndex to change to.</param>
		public static void ChangeTreeNodeImageIndex(MWTreeNodeWrapper mwtnw, int iImageIndex)
		{
			if(mwtnw != null)
			{
				if((mwtnw.Node.TreeView as MWTreeView).IsTreeNodeSelected(mwtnw.Node))
				{
					if(mwtnw != null)
					{
						mwtnw.ImageIndex = iImageIndex;
					}
				}
				else
				{
					mwtnw.Node.ImageIndex = iImageIndex;
				}
			}
		}

		#endregion ChangeTreeNodeImageIndex



		#region ChangeTreeNodeImageIndices

		/// <summary>
		/// Change the ImageIndex and SelectedImageIndex of the supplied TreeNode.
		/// </summary>
		/// <param name="tn">TreeNode to change the ImageIndex and SelectedImageIndex of.</param>
		/// <param name="iImageIndex">ImageIndex to change to.</param>
		/// <param name="iSelectedImageIndex">SelectedImageIndex to change to.</param>
		public static void ChangeTreeNodeImageIndices(TreeNode tn, int iImageIndex, int iSelectedImageIndex)
		{
			if(tn != null)
			{
				if((tn.TreeView as MWTreeView).IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = (tn.TreeView as MWTreeView).SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					mwtnw.ImageIndex = iImageIndex;
					mwtnw.SelectedImageIndex = iSelectedImageIndex;

					tn.ImageIndex = iSelectedImageIndex;
					tn.SelectedImageIndex = iSelectedImageIndex;
				}
				else
				{
					tn.ImageIndex = iImageIndex;
					tn.SelectedImageIndex = iSelectedImageIndex;
				}
			}
		}

		/// <summary>
		/// Change the ImageIndex and SelectedImageIndex of the supplied MWTreeNodeWrapper's TreeNode.
		/// </summary>
		/// <param name="tn">MWTreeNodeWrapper whose TreeNode to change the ImageIndex and SelectedImageIndex of.</param>
		/// <param name="iImageIndex">ImageIndex to change to.</param>
		/// <param name="iSelectedImageIndex">SelectedImageIndex to change to.</param>
		public static void ChangeTreeNodeImageIndices(MWTreeNodeWrapper mwtnw, int iImageIndex, int iSelectedImageIndex)
		{
			if(mwtnw != null)
			{
				if((mwtnw.Node.TreeView as MWTreeView).IsTreeNodeSelected(mwtnw.Node))
				{
					mwtnw.ImageIndex = iImageIndex;
					mwtnw.SelectedImageIndex = iSelectedImageIndex;

					mwtnw.Node.ImageIndex = iSelectedImageIndex;
					mwtnw.Node.SelectedImageIndex = iSelectedImageIndex;
				}
				else
				{
					mwtnw.Node.ImageIndex = iImageIndex;
					mwtnw.Node.SelectedImageIndex = iSelectedImageIndex;
				}
			}
		}

		#endregion ChangeTreeNodeImageIndices



		#region ChangeTreeNodeSelectedImageIndex

		/// <summary>
		/// Change the SelectedImageIndex of the supplied TreeNode.
		/// </summary>
		/// <param name="tn">TreeNode to change the SelectedImageIndex of.</param>
		/// <param name="iImageIndex">SelectedImageIndex to change to.</param>
		public static void ChangeTreeNodeSelectedImageIndex(TreeNode tn, int iSelectedImageIndex)
		{
			if(tn != null)
			{
				if((tn.TreeView as MWTreeView).IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = (tn.TreeView as MWTreeView).SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					mwtnw.SelectedImageIndex = iSelectedImageIndex;

					tn.ImageIndex = iSelectedImageIndex;
					tn.SelectedImageIndex = iSelectedImageIndex;
				}
				else
				{
					tn.SelectedImageIndex = iSelectedImageIndex;
				}
			}
		}

		/// <summary>
		/// Change the SelectedImageIndex of the supplied MWTreeNodeWrapper's TreeNode.
		/// </summary>
		/// <param name="tn">MWTreeNodeWrapper whose TreeNode to change the SelectedImageIndex of.</param>
		/// <param name="iSelectedImageIndex">SelectedImageIndex to change to.</param>
		public static void ChangeTreeNodeSelectedImageIndex(MWTreeNodeWrapper mwtnw, int iSelectedImageIndex)
		{
			if(mwtnw != null)
			{
				if((mwtnw.Node.TreeView as MWTreeView).IsTreeNodeSelected(mwtnw.Node))
				{
					mwtnw.SelectedImageIndex = iSelectedImageIndex;

					mwtnw.Node.ImageIndex = iSelectedImageIndex;
					mwtnw.Node.SelectedImageIndex = iSelectedImageIndex;
				}
				else
				{
					mwtnw.Node.SelectedImageIndex = iSelectedImageIndex;
				}
			}
		}

		#endregion ChangeTreeNodeSelectedImageIndex

		#endregion ChangeTreeNodeImageIndex, ChangeTreeNodeImageIndices and ChangeTreeNodeSelectedImageIndex



		#region ChangeBackColor, ChangeForeColor

		#region ChangeBackColor

		/// <summary>
		/// Change the BackColor of a TreeNode.
		/// This method handles selected as well as unselected TreeNodes.
		/// </summary>
		/// <param name="tn">TreeNode to change the BackColor of.</param>
		/// <param name="cBackColor">Color to change the supplied TreeNode's BackColor to.</param>
		public void ChangeBackColor(TreeNode tn, Color cBackColor)
		{
			if(tn != null)
			{
				if(this.IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = this.SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					if(mwtnw != null)
					{
						mwtnw.BackColor = cBackColor;

						if(	!this.Enabled && !this.UseExtendedDisabledColors ||
							this.Enabled && (this.Focused || !this.HideSelection) ||
							this.Enabled && !this.Focused && this.HideSelection)
						{
							mwtnw.Node.BackColor = cBackColor;
						}
					}
				}
				else
				{
					if(!this.Enabled)
					{
						MWTreeNodeWrapper mwtnw = GetDisabledMWTreeNodeWrapper(tn);

						if(mwtnw != null)
						{
							mwtnw.Enable();

							htDisabledTreeNodes.Remove(tn.GetHashCode());
						}

						tn.BackColor = cBackColor;

						DisableNode(tn);
					}
					else
					{
						tn.BackColor = cBackColor;
					}
				}
			}
		}

		#endregion ChangeBackColor



		#region ChangeForeColor

		/// <summary>
		/// Change the ForeColor of a TreeNode.
		/// This method handles selected as well as unselected TreeNodes.
		/// </summary>
		/// <param name="tn">TreeNode to change the ForeColor of.</param>
		/// <param name="cForeColor">Color to change the supplied TreeNode's ForeColor to.</param>
		public void ChangeForeColor(TreeNode tn, Color cForeColor)
		{
			if(tn != null)
			{
				if(this.IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = this.SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					if(mwtnw != null)
					{
						mwtnw.ForeColor = cForeColor;

						if(	!this.Enabled && !this.UseExtendedDisabledColors ||
							this.Enabled && (this.Focused || !this.HideSelection) ||
							this.Enabled && !this.Focused && this.HideSelection)
						{
							mwtnw.Node.ForeColor = cForeColor;
						}
					}
				}
				else
				{
					if(!this.Enabled)
					{
						MWTreeNodeWrapper mwtnw = GetDisabledMWTreeNodeWrapper(tn);

						if(mwtnw != null)
						{
							mwtnw.Enable();

							htDisabledTreeNodes.Remove(tn.GetHashCode());
						}

						tn.ForeColor = cForeColor;

						DisableNode(tn);
					}
					else
					{
						tn.ForeColor = cForeColor;
					}
				}
			}
		}

		#endregion ChangeForeColor

		#endregion ChangeBackColor, ChangeForeColor



		#region ChangeColors

		/// <summary>
		/// Change the colors (BackColor and ForeColor) of a TreeNode.
		/// This method handles selected as well as unselected TreeNodes.
		/// </summary>
		/// <param name="tn">TreeNode to change the colors of.</param>
		/// <param name="cBackColor">Color to change the supplied TreeNode's BackColor to.</param>
		/// <param name="cForeColor">Color to change the supplied TreeNode's ForeColor to.</param>
		public void ChangeColors(TreeNode tn, Color cBackColor, Color cForeColor)
		{
			if(tn != null)
			{
				if(this.IsTreeNodeSelected(tn))
				{
					MWTreeNodeWrapper mwtnw = this.SelNodes[tn.GetHashCode()] as MWTreeNodeWrapper;

					if(mwtnw != null)
					{
						mwtnw.BackColor = cBackColor;
						mwtnw.ForeColor = cForeColor;

						if(	!this.Enabled && !this.UseExtendedDisabledColors ||
							this.Enabled && (this.Focused || !this.HideSelection) ||
							this.Enabled && !this.Focused && this.HideSelection)
						{
							mwtnw.Node.BackColor = cBackColor;
							mwtnw.Node.ForeColor = cForeColor;
						}
					}
				}
				else
				{
					if(!this.Enabled)
					{
						MWTreeNodeWrapper mwtnw = GetDisabledMWTreeNodeWrapper(tn);

						if(mwtnw != null)
						{
							mwtnw.Enable();

							htDisabledTreeNodes.Remove(tn.GetHashCode());
						}

						tn.BackColor = cBackColor;
						tn.ForeColor = cForeColor;

						DisableNode(tn);
					}
					else
					{
						tn.BackColor = cBackColor;
						tn.ForeColor = cForeColor;
					}
				}
			}
		}

		#endregion ChangeColors



		#region Rectangle Modification Methods

		#region GetRectangleOneSmaller

		/// <summary>
		/// Returns a Rectangle that has the same Location (X and Y) but with the width and height each decreased by 1.
		/// Note that this is useful for calling DrawRectangle on the 'same' Rectangle as FillRectangle was called.
		/// </summary>
		/// <param name="rct">Rectangle to decrease the width and height of by 1.</param>
		/// <returns>Rectangle that has the same Location (X and Y) but with the width and height each decreased by 1.</returns>
		public static Rectangle GetRectangleOneSmaller(Rectangle rct)
		{
			return new Rectangle(rct.X, rct.Y, rct.Width - 1, rct.Height - 1);
		}

		#endregion GetRectangleOneSmaller



		#region GetPositiveRectangle (2 overloads)

		/// <summary>
		/// Get a Rectangle that has positive width and positive height based on the coordinates, width and height supplied.
		/// </summary>
		/// <param name="x">X-coordinate of a Rectangle.</param>
		/// <param name="y">Y-coordinate of a Rectangle.</param>
		/// <param name="w">Width of a Rectangle.</param>
		/// <param name="h">Height of a Rectangle.</param>
		/// <returns>Rectangle that has positive width and positive height based on the coordinates, width and height supplied.</returns>
		public static Rectangle GetPositiveRectangle(int x, int y, int w, int h)
		{
			Rectangle rctReturn;

			if(w < 0 && h < 0)
			{
				rctReturn = new Rectangle(x + w, y + h, -w, -h);
			}
			else if(w < 0)
			{
				rctReturn = new Rectangle(x + w, y, -w, h);
			}
			else if(h < 0)
			{
				rctReturn = new Rectangle(x, y + h, w, -h);
			}
			else
			{
				rctReturn = new Rectangle(x, y, w, h);
			}

			return rctReturn;
		}

		/// <summary>
		/// Get a Rectangle that has a positive Location and the same position and positive width and positive height as the Rectangle supplied.
		/// </summary>
		/// <param name="rct">Rectangle that should be made positive.</param>
		/// <returns>Rectangle that has a positive Location and the same position and positive width and positive height as the Rectangle supplied.</returns>
		public static Rectangle GetPositiveRectangle(Rectangle rct)
		{
			Rectangle rctReturn = rct;

			if(rct.Width < 0 && rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width, rct.Y + rct.Height, -rct.Width, -rct.Height);
			}
			else if(rct.Width < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width, rct.Y, -rct.Width, rct.Height);
			}
			else if(rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X, rct.Y + rct.Height, rct.Width, -rct.Height);
			}

			return rctReturn;
		}

		#endregion GetPositiveRectangle (2 overloads)



		#region GetPositiveRectangleOneSmaller

		/// <summary>
		/// Get a Rectangle that has a positive Location and the same position and positive width - 1 and positive height - 1 as the Rectangle supplied.
		/// </summary>
		/// <param name="rct">Rectangle that should be made positive.</param>
		/// <returns>Rectangle that has a positive Location and the same position and positive width - 1 and positive height - 1 as the Rectangle supplied.</returns>
		public static Rectangle GetPositiveRectangleOneSmaller(Rectangle rct)
		{
			Rectangle rctReturn = rct;

			if(rct.Width < 0 && rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width, rct.Y + rct.Height, -rct.Width - 1, -rct.Height - 1);
			}
			else if(rct.Width < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width, rct.Y, -rct.Width - 1, rct.Height - 1);
			}
			else if(rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X, rct.Y + rct.Height, rct.Width - 1, -rct.Height - 1);
			}
			else
			{
				rctReturn = new Rectangle(rct.X, rct.Y, rct.Width - 1, rct.Height - 1);
			}

			return rctReturn;
		}

		#endregion GetPositiveRectangleOneSmaller



		#region GetPositiveRectangleSmaller

		/// <summary>
		/// Get a Rectangle that has a positive Location and the same position and positive width minus thickness (supplied) divided by two and positive height minus thickness (supplied) divided by two as the Rectangle supplied.
		/// Note that the Rectangle is changed by the thickness divided by two.
		/// </summary>
		/// <param name="rct">Rectangle that should be made positive.</param>
		/// <returns>Rectangle that has a positive Location and the same position and positive width minus thickness (supplied) divided by two and positive height minus thickness (supplied) divided by two as the Rectangle supplied.\nNote that the Rectangle is changed by the thickness divided by two.</returns>
		public static Rectangle GetPositiveRectangleSmaller(Rectangle rct, int iThickness)
		{
			Rectangle rctReturn = rct;

			int iThicknessDiv2 = iThickness / 2;

			if(rct.Width < 0 && rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width + iThicknessDiv2, rct.Y + rct.Height + iThicknessDiv2, -rct.Width - iThickness, -rct.Height - iThickness);
			}
			else if(rct.Width < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width + iThicknessDiv2, rct.Y + iThicknessDiv2, -rct.Width - iThickness, rct.Height - iThickness);
			}
			else if(rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X + iThicknessDiv2, rct.Y + rct.Height + iThicknessDiv2, rct.Width - iThickness, -rct.Height - iThickness);
			}
			else
			{
				rctReturn = new Rectangle(rct.X + iThicknessDiv2, rct.Y + iThicknessDiv2, rct.Width - iThickness, rct.Height - iThickness);
			}

			return rctReturn;
		}

		#endregion GetPositiveRectangleSmaller



		#region GetPositiveRectangleInside

		/// <summary>
		/// Get a Rectangle that has a positive Location and the same position and positive width minus thickness (supplied) and positive height minus thickness (supplied) as the Rectangle supplied.
		/// </summary>
		/// <param name="rct">Rectangle that should be made positive.</param>
		/// <returns>Rectangle that has a positive Location and the same position and positive width minus thickness (supplied) and positive height minus thickness (supplied) as the Rectangle supplied.</returns>
		public static Rectangle GetPositiveRectangleInside(Rectangle rct, int iThickness)
		{
			Rectangle rctReturn = rct;

			if(rct.Width < 0 && rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width + iThickness, rct.Y + rct.Height + iThickness, -rct.Width - 2 * iThickness, -rct.Height - 2 * iThickness);
			}
			else if(rct.Width < 0)
			{
				rctReturn = new Rectangle(rct.X + rct.Width + iThickness, rct.Y, -rct.Width - 2 * iThickness, rct.Height - 2 * iThickness);
			}
			else if(rct.Height < 0)
			{
				rctReturn = new Rectangle(rct.X, rct.Y + rct.Height + iThickness, rct.Width - 2 * iThickness, -rct.Height - 2 * iThickness);
			}
			else
			{
				rctReturn = new Rectangle(rct.X + iThickness, rct.Y + iThickness, rct.Width - 2 * iThickness, rct.Height - 2 * iThickness);
			}

			return rctReturn;
		}

		#endregion GetPositiveRectangleInside

		#endregion Rectangle Modification Methods

		#endregion Modification Public Static Help Methods

		#endregion Public Static Help Methods



		#region Private Static Help Methods

		#region IsPlainColoredTreeNode

		/// <summary>
		/// Checks if a TreeNode is plain colored or not.
		/// Plain colored is defined as:
		///		BackColor is SystemColors.Control
		///		ForeColor is SystemColors.ControlDark
		/// </summary>
		/// <param name="tn">TreeNode to be checked if it is plain colored or not.</param>
		/// <returns>True if a TreeNode is plain colored or false otherwise.</returns>
		private static bool IsPlainColoredTreeNode(TreeNode tn)
		{
			return
				tn.BackColor == SystemColors.Control &&
				tn.ForeColor == SystemColors.ControlDark;
		}

		#endregion IsPlainColoredTreeNode

		#endregion Private Static Help Methods

	}
}
