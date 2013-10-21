using System;

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
	#region StringFormatEnum

	/// <summary>
	/// Decides the base StringFormat.
	/// </summary>
	public enum StringFormatEnum
	{
		GenericDefault = 0,
		GenericTypographic = 1
	}

	#endregion StringFormatEnum



	#region TreeViewMultiSelect Enum

	/// <summary>
	/// Decides the multi select characteristics of an MWTreeView Control.
	/// </summary>
	public enum TreeViewMultiSelect
	{
		/// <summary>
		/// Classic single select.
		/// </summary>
		Classic = 0,

		/// <summary>
		/// Enhanced single select.
		/// </summary>
		NoMulti = 1,

		/// <summary>
		/// Standard multi select with no particular constraints.
		/// </summary>
		Multi = 2,

		/// <summary>
		/// Multi select where the selected TreeNodes have to reside on the same branch and level.
		/// </summary>
		MultiSameBranchAndLevel = 3,

		/// <summary>
		/// Multi select where the selected TreeNodes have to reside on the same branch.
		/// </summary>
		MultiSameBranch = 4,

		/// <summary>
		/// Multi select where the selected TreeNodes have to reside on the same level.
		/// </summary>
		MultiSameLevel = 5,

		/// <summary>
		/// Multi select where the selected TreeNodes also select all their direct parents.
		/// Multiple grandparents are NOT allowed, i.e. multiple selected TreeNodes at level 0 are NOT allowed.
		/// </summary>
		MultiPathToParent = 6,

		/// <summary>
		/// Multi select where the selected TreeNodes also select all their direct parents.
		/// Multiple grandparents ARE allowed, i.e. multiple selected TreeNodes at level 0 ARE allowed.
		/// </summary>
		MultiPathToParents = 7,

		/// <summary>
		/// Essentially a single select where the TreeNode's direct parents are selected.
		/// Multiple grandparents are NOT allowed, i.e. multiple selected TreeNodes at level 0 are NOT allowed.
		/// </summary>
		SinglePathToParent = 8,

		/// <summary>
		/// Essentially a single select per grandparent branch where the TreeNode's direct parents are selected.
		/// Multiple grandparents ARE allowed, i.e. multiple selected TreeNodes at level 0 ARE allowed.
		/// </summary>
		SinglePathToParents = 9,

		/// <summary>
		/// Multi select where all child TreeNodes are selected.
		/// Note that similar results might be obtained by using Multi and holding down the Alt key - but here there are constraints to which TreeNodes can be deselected etc.
		/// </summary>
		MultiAllDescendants = 10
	}

	#endregion TreeViewMultiSelect Enum



	#region RubberbandSelectionMode Enum

	/// <summary>
	/// Decides which rubberband selection mode is used when dragging the mouse (while holding down the left mouse button) to select TreeNodes.
	/// </summary>
	public enum RubberbandSelectionMode
	{
		/// <summary>
		/// No rubberband selection is allowed.
		/// </summary>
		None = 0,

		/// <summary>
		/// Dashed thin line rubberband.
		/// </summary>
		RubberbandDashed = 1,

		/// <summary>
		/// Solid thick line rubberband.
		/// </summary>
		RubberbandSolid = 2,

		/// <summary>
		/// A selection frame.
		/// </summary>
		SelectionFrame = 3,

		/// <summary>
		/// A solid frame.
		/// </summary>
		Frame = 4,

		/// <summary>
		/// Fill color.
		/// </summary>
		Fill = 5,

		/// <summary>
		/// Hatch style fill.
		/// </summary>
		Hatch = 6,

		/// <summary>
		/// Texture fill.
		/// </summary>
		//Texture = 7,

		/// <summary>
		/// Gradient fill.
		/// </summary>
		Gradient = 8,

		/// <summary>
		/// Path gradient fill.
		/// </summary>
		//PathGradient = 9,

		/// <summary>
		/// Custom rubberband painting mode.
		/// </summary>
		Custom = 10,

		/// <summary>
		/// Plain image as opposed to a texture.
		/// </summary>
		Image = 11
	}

	#endregion RubberbandSelectionMode Enum



	#region GradientPaintMode Enum

	/// <summary>
	/// Decides which paint mode is used when painting a Gradient rubberband.
	/// </summary>
	public enum GradientPaintMode
	{
		/// <summary>
		/// The starting point and end point of the rubberband Gradient are used.
		/// </summary>
		MousePoints = 0,

		/// <summary>
		/// An angle is used.
		/// </summary>
		Angle = 1,

		/// <summary>
		/// A LinearGradientMode is used.
		/// </summary>
		LinearGradientMode = 2
	}

	#endregion GradientPaintMode Enum



	#region GradientShape Enum

	/// <summary>
	/// Decides the shape that is applied to the Gradient rubberband
	/// </summary>
	public enum GradientShape
	{
		/// <summary>
		/// Regular two color linear gradient.
		/// </summary>
		Regular = 0,

		/// <summary>
		/// Linear gradient with a center color and a linear falloff to a single color on both ends.
		/// </summary>
		Triangular = 1,

		/// <summary>
		/// Non-linear gradient falloff based on a bell-shaped curve.
		/// </summary>
		SigmaBell = 2,

		/// <summary>
		/// Custom blend shape.
		/// </summary>
		Blend = 3,

		/// <summary>
		/// Custom color blend shape.
		/// </summary>
		ColorBlend = 4
	}

	#endregion GradientShape Enum



	#region MouseClearSelNodes

	/// <summary>
	/// This is used when deciding whether a click on the MWTreeView is on a blank area or not.
	/// Note that some of the MouseClearSelNodes enum members or part of what they represent are ignored if the FullRowSelect property of the MWTreeView is set to true.
	/// </summary>
	public enum MouseClearSelNodes
	{
		/// <summary>
		/// Clicking the mouse does not deselect TreeNodes.
		/// </summary>
		None = 0,

		/// <summary>
		/// Clicking below all TreeNodes deselects all TreeNodes.
		/// </summary>
		Bottom = 1,

		/// <summary>
		/// Clicking to the left of the TreeNodes deselects all TreeNodes.
		/// Note that clicking the expand/collapse symbol to the left of the TreeNode (if there is one) is not considered to be a reason for deselecting all TreeNodes.
		/// </summary>
		LeftSide = 2,

		/// <summary>
		/// Clicking to the right of the TreeNodes deselects all TreeNodes.
		/// </summary>
		RightSide = 3,

		/// <summary>
		/// Clicking either to the left or right of the TreeNodes deselects all TreeNodes.
		/// Note that clicking the expand/collapse symbol to the left of the TreeNode (if there is one) is not considered to be a reason for deselecting all TreeNodes.
		/// </summary>
		BothSides = 4,

		/// <summary>
		/// Clicking to the left of the TreeNodes or below all TreeNodes deselects all TreeNodes.
		/// Note that clicking the expand/collapse symbol to the left of the TreeNode (if there is one) is not considered to be a reason for deselecting all TreeNodes.
		/// </summary>
		BottomAndLeftSide = 5,

		/// <summary>
		/// Clicking to the right of the TreeNodes or below all TreeNodes deselects all TreeNodes.
		/// </summary>
		BottomAndRightSide = 6,

		/// <summary>
		/// Clicking either to the left or right of the TreeNodes deselects all TreeNodes.
		/// Note that clicking the expand/collapse symbol to the left of the TreeNode (if there is one) is not considered to be a reason for deselecting all TreeNodes.
		/// </summary>
		BottomAndBothSides = 7
	}

	#endregion MouseClearSelNodes

}
