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
	/// <summary>
	/// Summary description for MWRubberbandGradientBlend.
	/// </summary>
	public class MWRubberbandGradientBlend
	{
		#region Variables

		/// <summary>
		/// Blend factor for the gradient.
		/// </summary>
		private float fFactor = 0.0f;

		/// <summary>
		/// Blend position for the gradient.
		/// </summary>
		private float fPosition = 0.0f;

		#endregion Variables



		#region Constructors

		/// <summary>
		/// Standard constructor used by the Visual Studio designer.
		/// </summary>
		public MWRubberbandGradientBlend()
		{
		}

		/// <summary>
		/// Standard constructor.
		/// </summary>
		/// <param name="fFactor">Blend factor for the gradient.</param>
		/// <param name="fPosition">Blend position for the gradient.</param>
		public MWRubberbandGradientBlend(float fFactor, float fPosition)
		{
			this.fFactor = fFactor;
			this.fPosition = fPosition;
		}

		#endregion Constructors



		#region Properties

		/// <summary>
		/// Blend factor for the gradient.
		/// </summary>
		public float Factor
		{
			get
			{
				return fFactor;
			}
			set
			{
				if(value >= 0.0f && value <= 1.0f)
				{
					fFactor = value;
				}
			}
		}

		/// <summary>
		/// Blend position for the gradient.
		/// </summary>
		public float Position
		{
			get
			{
				return fPosition;
			}
			set
			{
				if(value >= 0.0f && value <= 1.0f)
				{
					fPosition = value;
				}
			}
		}

		#endregion Properties



		#region Public Static Methods

		/// <summary>
		/// Get an array of blend factors to be used for a gradient.
		/// </summary>
		/// <param name="amwrgb">Array of MWRubberbandGradientBlends where the factors should be extracted from.</param>
		/// <returns>An array of blend factors to be used for a gradient.</returns>
		public static float[] GetFactorArray(MWRubberbandGradientBlend[] amwrgb)
		{
			float[] af = new float[amwrgb.Length];

			for(int i = 0; i < amwrgb.Length; i++)
			{
				af[i] = amwrgb[i].Factor;
			}

			return af;
		}

		/// <summary>
		/// Get an array of blend positions to be used for a gradient.
		/// </summary>
		/// <param name="amwrgb">Array of MWRubberbandGradientBlends where the positions should be extracted from.</param>
		/// <returns>An array of blend positions to be used for a gradient.</returns>
		public static float[] GetPositionArray(MWRubberbandGradientBlend[] amwrgb)
		{
			float[] af = new float[amwrgb.Length];

			for(int i = 0; i < amwrgb.Length; i++)
			{
				af[i] = amwrgb[i].Position;
			}

			return af;
		}

		#endregion Public Static Methods

	}
}
