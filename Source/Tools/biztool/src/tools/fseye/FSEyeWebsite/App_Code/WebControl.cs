using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Drawing;

namespace FSEye.WebControls
{
	/// <summary>
	/// 进度条控件
	/// </summary>
	/// <author>徐晓刚</author>
	public class ProgressBar : WebControl
	{
		Orientation _orientation = Orientation.Horizontal;

		double _progress = 0;

		bool _showIndicator = true;

		string _processedCssClass = string.Empty;

		string _indicatorCssClass = string.Empty;

		string _completedCssClass = string.Empty;
		
		public override Unit Width
		{
			get { return base.Width; }
			set
			{

				if (value.Type == UnitType.Pixel)
					base.Width = value;
				else
					throw new Exception("This control only supports Pixel Unit!");
			}
		}

		public override Unit Height
		{
			get { return base.Height; }
			set
			{
				if (value.Type == UnitType.Pixel)
					base.Height = value;
				else
					throw new Exception("This control only supports Pixel Unit!");				
			}
		}

		/// <summary>
		/// 方向
		/// </summary>
		public Orientation Orientation
		{
			get { return _orientation; }
			set { _orientation = value; }
		}

		/// <summary>
		/// 进度
		/// </summary>
		/// <remarks>取值范围：0~1</remarks>
		public double Progress
		{
			get { return _progress; }
			set
			{
                if (value >= 0 && value <= 1)
                {
                   _progress = value;                   
                }
                else
                    throw new Exception("Progress must between 0 and 1!");
			}
		}

		/// <summary>
		/// 显示进度指示
		/// </summary>
		public bool ShowIndicator
		{
			get { return _showIndicator; }
			set { _showIndicator = value; }
		}

		/// <summary>
		/// 完成部分的CSS样式类
		/// </summary>
		public string ProcessedCssClass
		{
			get { return _processedCssClass; }
			set { _processedCssClass = value; }
		}

		/// <summary>
		/// 进度指示CSS样式类
		/// </summary>
		public string IndicatorCssClass
		{
			get { return _indicatorCssClass; }
			set { _indicatorCssClass = value; }
		}

		/// <summary>
		/// 完成后的CSS样式类
		/// </summary>
		public string CompletedCssClass
		{
			get { return _completedCssClass; }
			set { _completedCssClass = value; }
		}        

		public ProgressBar()
		{
		}

		protected override void RenderContents(HtmlTextWriter writer)
		{
			writer.Write("<div style='position:relative;'>");			

			string processedCssClassStr = string.Empty;
			if (_progress == 1)
			{
				if (_completedCssClass != null && _completedCssClass.Length > 0)
				{
					processedCssClassStr = " class=" + _completedCssClass;
				}
			}
			else
			{
				if (_processedCssClass != null && _processedCssClass.Length > 0)
				{
					processedCssClassStr = " class=" + _processedCssClass;
				}
			}

			if (_orientation == Orientation.Horizontal)
			{
				writer.Write("<div style='position:absolute; width: {0}; height: {1}'{2} />",
					new Unit(Math.Round(Width.Value * _progress), Width.Type),
					Height, 
					processedCssClassStr);
			}
			else if (_orientation == Orientation.Vertical)
			{
                //#############################################################
                //用overflow:hidden来控制IE6高度自动扩大的问题
                //#############################################################
                string childDiv = string.Format("<div style='position:absolute; width: {0}; height: {1}; top: 0;' />",
                                  Width,
				                  new Unit(Height.Value, Height.Type));		                  
				                  
                string parentDiv = string.Format("<div style='position:absolute; width: {0}; height: {1}; top: {2}; overflow:hidden;'{3} >{4}</div>",
                                   Width,
                                   new Unit(Math.Round(Height.Value * _progress),Height.Type),
                                   new Unit(Math.Round(Height.Value * (1-_progress)),Height.Type),
                                   processedCssClassStr,
                                   childDiv);

                writer.Write(parentDiv);
			}

			if (_showIndicator)
			{
				string indicatorCssClassStr = string.Empty;
				if (_indicatorCssClass != null && _indicatorCssClass.Length > 0)
				{
					indicatorCssClassStr = " class=" + _indicatorCssClass;
				}
				writer.Write("<div style='position:absolute; left: {0}; top: {1};'{2}>{3}%</div>", new Unit(Width.Value / 2 - 8, UnitType.Pixel), new Unit(Height.Value / 2 - 5, UnitType.Pixel), indicatorCssClassStr, (int)(_progress * 100));
			}

			writer.Write("</div>");
		}
	}
}
