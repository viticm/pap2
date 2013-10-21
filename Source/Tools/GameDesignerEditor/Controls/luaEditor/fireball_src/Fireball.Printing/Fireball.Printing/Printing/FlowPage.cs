using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Printing
{
    public sealed class FlowPage
    {
        private List<FlowRow> _Rows = new List<FlowRow>();

        public List<FlowRow> Rows
        {
            get
            {
                return _Rows;
            }
            set
            {
                _Rows = value;
            }
        }
    }
}
