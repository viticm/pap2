using System;
using System.Drawing;
using System.Drawing.Drawing2D;

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
	#region Class MWCancelEventArgs and Delegate MWCancelEventHandler

	/// <summary>
	/// A delegate for event MWCancelEventHandler.
	/// </summary>
	public delegate void MWCancelEventHandler(object sender, MWCancelEventArgs e);

	/// <summary>
	/// MWCancelEventArgs class.
	/// The MWCancelEventArgs takes two objects as arguments. These two objects are the current value and the proposed value. These objects
	///		can be used when setting up EventHandlers for the properties that use them so that the programmer will know what the current
	///		and proposed values are.
	///	Note that the MWCancelEventArgs should be used in an OnBeforePROPERTYChanged property - BEFORE the value of the property is changed.
	/// </summary>
	public class MWCancelEventArgs : System.ComponentModel.CancelEventArgs
	{
		#region Variables

		/// <summary>
		/// The current object before the property is changed.
		/// </summary>
		private object oCurrent = null;

		/// <summary>
		/// The proposed object that will be used if the property is changed.
		/// </summary>
		private object oProposed = null;

		#endregion Variables



		#region Constructors

		/// <summary>
		/// Standard constructor.
		/// </summary>
		public MWCancelEventArgs()
		{
		}

		/// <summary>
		/// Standard Constructor taking the current value of the property and the proposed value of the property as arguments.
		/// </summary>
		/// <param name="current">The current object before the property is changed.</param>
		/// <param name="proposed">The proposed object that will be used if the property is changed.</param>
		public MWCancelEventArgs(object current, object proposed)
		{
			oCurrent = current;
			oProposed = proposed;
		}

		#endregion Constructors



		#region Properties

		#region Current Property

		/// <summary>
		/// The current object before the property is changed.
		/// </summary>
		public object Current
		{
			get
			{
				return oCurrent;
			} 
		}

		#endregion Current Property



		#region Proposed Property

		/// <summary>
		/// The proposed object that will be used if the property is changed.
		/// </summary>
		public object Proposed
		{
			get
			{
				return oProposed;
			} 
		}

		#endregion Proposed Property

		#endregion Properties

	}

	#endregion Class MWCancelEventArgs and Delegate MWCancelEventHandler





	#region Class MWPropertyEventArgs and Delegate MWPropertyEventHandler

	/// <summary>
	/// A delegate for event MWPropertyEventHandler.
	/// </summary>
	public delegate void MWPropertyEventHandler(object sender, MWPropertyEventArgs e);

	/// <summary>
	/// MWPropertyEventArgs class.
	/// The MWPropertyEventArgs takes one object as argument. This object is the updated value. This object can be used when setting up EventHandlers
	///		for the properties that use them so that the programmer will know what the updated value is.
	///	Note that the MWPropertyEventArgs should be used in an OnAfterPROPERTYChanged property - AFTER the value of the property is changed.
	/// </summary>
	public class MWPropertyEventArgs : System.EventArgs
	{
		#region Variables

		/// <summary>
		/// The updated object after the property has changed.
		/// </summary>
		private object oUpdated = null;

		#endregion Variables



		#region Constructors

		/// <summary>
		/// Standard constructor.
		/// </summary>
		public MWPropertyEventArgs()
		{
		}

		/// <summary>
		/// Standard Constructor taking the updated value of the property as argument.
		/// </summary>
		/// <param name="updated">The updated object after the property has changed.</param>
		public MWPropertyEventArgs(object updated)
		{
			oUpdated = updated;
		}

		#endregion Constructors



		#region Properties

		/// <summary>
		/// The updated object after the property has changed.
		/// </summary>
		public object Updated
		{
			get
			{
				return oUpdated;
			} 
		}

		#endregion Properties

	}

	#endregion Class MWPropertyEventArgs and Delegate MWPropertyEventHandler





	#region Class MWHatchStyleEventArgs and Delegate MWHatchStyleEventHandler

	/// <summary>
	/// A delegate for event MWHatchStyleEventHandler.
	/// </summary>
	public delegate void MWHatchStyleEventHandler(object sender, MWHatchStyleEventArgs e);

	/// <summary>
	/// MWHatchStyleEventArgs class.
	/// </summary>
	public class MWHatchStyleEventArgs : System.EventArgs 
	{
		#region Variables

		/// <summary>
		/// The old HatchStyle before the property was changed.
		/// </summary>
		private HatchStyle hsOldHatchStyle = HatchStyle.Weave;

		/// <summary>
		/// The new HatchStyle after the property was changed.
		/// </summary>
		private HatchStyle hsNewHatchStyle = HatchStyle.Weave;

		#endregion Variables



		#region Constructor

		/// <summary>
		/// Standard Constructor.
		/// </summary>
		/// <param name="hsOld">The old HatchStyle before the property was changed.</param>
		/// <param name="hsNew">The new HatchStyle after the property was changed.</param>
		public MWHatchStyleEventArgs(HatchStyle hsOld, HatchStyle hsNew)
		{
			hsOldHatchStyle = hsOld;
			hsNewHatchStyle = hsNew;
		}

		#endregion Constructor



		#region Properties

		#region OldHatchStyle Property

		/// <summary>
		/// The old HatchStyle before the property was changed.
		/// </summary>
		public HatchStyle OldHatchStyle
		{
			get
			{
				return hsOldHatchStyle;
			} 
		}

		#endregion OldHatchStyle Property



		#region NewHatchStyle Property

		/// <summary>
		/// The new HatchStyle after the property was changed.
		/// </summary>
		public HatchStyle NewHatchStyle
		{
			get
			{
				return hsNewHatchStyle;
			} 
		}

		#endregion NewHatchStyle Property

		#endregion Properties
	}

	#endregion Class MWHatchStyleEventArgs and Delegate MWHatchStyleEventHandler





	#region Class MWRubberbandPaintEventArgs and Delegate MWRubberbandPaintEventHandler

	/// <summary>
	/// A delegate for event MWRubberbandPaintEventHandler.
	/// </summary>
	public delegate void MWRubberbandPaintEventHandler(object sender, MWRubberbandPaintEventArgs e);

	/// <summary>
	/// MWRubberbandPaintEventArgs class.
	/// </summary>
	public class MWRubberbandPaintEventArgs : System.EventArgs 
	{
		#region Variables

		/// <summary>
		/// Graphics object to draw on.
		/// </summary>
		private Graphics g = null;

		/// <summary>
		/// Inner area to draw in represented in client coordinates.
		/// The inner area (InnerArea property) should be used to draw in when the MWTreeViewPaintBorder property is set to true,
		///		otherwise the outer area (OuterArea property) should be used.
		/// </summary>
		private Rectangle rctInnerArea = Rectangle.Empty;

		/// <summary>
		/// Outer area to draw in represented in client coordinates.
		/// The outer area (OuterArea property) should be used to draw in when the MWTreeViewPaintBorder property is set to false,
		///		otherwise the inner area (InnerArea property) should be used.
		/// </summary>
		private Rectangle rctOuterArea = Rectangle.Empty;

		/// <summary>
		/// Start point of the rubberband selection represented in client coordinates.
		/// In rough terms this is where the mouse was when the rubberband started being painted.
		/// </summary>
		private Point ptStartPoint = Point.Empty;

		/// <summary>
		/// End point of the rubberband selection represented in client coordinates.
		/// In rough terms this is where the mouse is now while the rubberband is being painted.
		/// </summary>
		private Point ptEndPoint = Point.Empty;

		/// <summary>
		/// True if the MWTreeView should handle painting the border that it is set up to do or false otherwise.
		/// If this property is false use the outer area (OuterArea property) to draw in.
		/// If this property is true use the inner area (InnerArea property) to draw in.
		/// </summary>
		private bool bMWTreeViewPaintBorder = true;

		#endregion Variables



		#region Constructor

		/// <summary>
		/// Standard Constructor.
		/// </summary>
		/// <param name="g">Graphics object to draw on.</param>
		/// <param name="rctInnerArea">Inner area to draw in.</param>
		/// <param name="rctOuterArea">Outer area to draw in.</param>
		/// <param name="ptStartPoint">Start point of the rubberband selection.</param>
		/// <param name="ptEndPoint">End point of the rubberband selection.</param>
		/// <param name="bMWTreeViewPaintBorder">True if the MWTreeView should handle painting the border that it is set up to do or false otherwise.</param>
		public MWRubberbandPaintEventArgs(Graphics g, Rectangle rctInnerArea, Rectangle rctOuterArea, Point ptStartPoint, Point ptEndPoint, bool bMWTreeViewPaintBorder)
		{
			this.g = g;
			this.rctInnerArea = new Rectangle(rctInnerArea.Location, rctInnerArea.Size);
			this.rctOuterArea = new Rectangle(rctOuterArea.Location, rctOuterArea.Size);
			this.ptStartPoint = new Point(ptStartPoint.X, ptStartPoint.Y);
			this.ptEndPoint = new Point(ptEndPoint.X, ptEndPoint.Y);
			this.bMWTreeViewPaintBorder = bMWTreeViewPaintBorder;
		}

		#endregion Constructor



		#region Properties

		/// <summary>
		/// Graphics object to draw on.
		/// </summary>
		public Graphics Graphics
		{
			get
			{
				return g;
			}
		}

		/// <summary>
		/// Inner area to draw in represented in client coordinates.
		/// The inner area (InnerArea property) should be used to draw in when the MWTreeViewPaintBorder property is set to true,
		///		otherwise the outer area (OuterArea property) should be used.
		/// </summary>
		public Rectangle InnerArea
		{
			get
			{
				return rctInnerArea;
			}
		}

		/// <summary>
		/// Outer area to draw in represented in client coordinates.
		/// The outer area (OuterArea property) should be used to draw in when the MWTreeViewPaintBorder property is set to false,
		///		otherwise the inner area (InnerArea property) should be used.
		/// </summary>
		public Rectangle OuterArea
		{
			get
			{
				return rctOuterArea;
			}
		}

		/// <summary>
		/// Start point of the rubberband selection represented in client coordinates.
		/// In rough terms this is where the mouse was when the rubberband started being painted.
		/// </summary>
		public Point StartPoint
		{
			get
			{
				return ptStartPoint;
			}
		}

		/// <summary>
		/// End point of the rubberband selection represented in client coordinates.
		/// In rough terms this is where the mouse is now while the rubberband is being painted.
		/// </summary>
		public Point EndPoint
		{
			get
			{
				return ptEndPoint;
			}
		}

		/// <summary>
		/// True if the MWTreeView should handle painting the border that it is set up to do or false otherwise.
		/// If this property is false use the outer area (OuterArea property) to draw in.
		/// If this property is true use the inner area (InnerArea property) to draw in.
		/// </summary>
		public bool MWTreeViewPaintBorder
		{
			get
			{
				return bMWTreeViewPaintBorder;
			}
			set
			{
				bMWTreeViewPaintBorder = value;
			}
		}

		#endregion Properties
	}

	#endregion Class MWRubberbandPaintEventArgs and Delegate MWRubberbandPaintEventHandler





	#region Class MWRubberbandClearEventArgs and Delegate MWRubberbandClearEventHandler

	/// <summary>
	/// A delegate for event MWRubberbandClearEventHandler.
	/// </summary>
	public delegate void MWRubberbandClearEventHandler(object sender, MWRubberbandClearEventArgs e);

	/// <summary>
	/// MWRubberbandClearEventArgs class.
	/// </summary>
	public class MWRubberbandClearEventArgs : System.EventArgs 
	{
		#region Variables

		/// <summary>
		/// Area that the MWTreeView should clear.
		/// </summary>
		private Rectangle rctClearArea = Rectangle.Empty;

		/// <summary>
		/// Area that the MWTreeView has painted on.
		/// Note that this could be used to minimize cleared areas.
		/// </summary>
		private Rectangle rctPaintArea = Rectangle.Empty;

		#endregion Variables



		#region Constructor

		/// <summary>
		/// Standard Constructor.
		/// </summary>
		/// <param name="rctInnerArea">Area that the MWTreeView intends to clear.</param>
		/// <param name="rctPaintArea">Area that the MWTreeView has painted on.</param>
		public MWRubberbandClearEventArgs(Rectangle rctClearArea, Rectangle rctPaintArea)
		{
			this.rctClearArea = new Rectangle(rctClearArea.Location, rctClearArea.Size);
			this.rctPaintArea = new Rectangle(rctPaintArea.Location, rctPaintArea.Size);
		}

		#endregion Constructor



		#region Properties

		/// <summary>
		/// Area that the MWTreeView should clear.
		/// </summary>
		public Rectangle ClearArea
		{
			get
			{
				return rctClearArea;
			}
			set
			{
				rctClearArea = value;
			}
		}

		/// <summary>
		/// Area that the MWTreeView has painted on.
		/// Note that this could be used to minimize cleared areas.
		/// </summary>
		public Rectangle PaintArea
		{
			get
			{
				return rctPaintArea;
			}
		}

		#endregion Properties
	}

	#endregion Class MWRubberbandClearEventArgs and Delegate MWRubberbandClearEventHandler

}
