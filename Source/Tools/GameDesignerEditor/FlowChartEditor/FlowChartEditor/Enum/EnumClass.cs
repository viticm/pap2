using System;
using System.Collections.Generic;
using System.Text;

namespace FlowChartEditor
{
    /// <summary>
    /// 方向枚举
    /// </summary>
    public enum Direction
    {
        Up, // 上
        Down, // 下
        Left, // 左
        Right, // 右
        Up_Left, // 左上
        Up_Right, // 右上
        Down_Left, // 左下
        Down_Right, // 右下
        None // 空
    }

    /// <summary>
    /// 编辑类型枚举
    /// </summary>
    public enum EditMode
    {
        Cut,
        Copy,
        Normal
    }

    /// <summary>
    /// 图元类型枚举
    /// </summary>
    public enum GraphType
    {
        ConditionNode, // 条件结点 
        ActionNode, // 动作结点
        EventNode, // 事件结点        
        Line, // 连接线
        AIActionNode, // AI动作结点
        AIActionsNode, // AI动作组结点
        AIStateNode, // AI状态结点
        AILine, // AI连接线
        InnerChart, // 子绘图
        InterfaceNode // 接口结点
    }

    /// <summary>
    /// 逻辑操作类型枚举
    /// </summary>
    public enum LogicType
    {
        CreateAtPoint, // 在指定位置创建图元(右键菜单中)
        InitializeDataElement, // 初始化数据元
        Connect, // 连接图元
        Disconnect, // 解除连接图元
        BeforeDelete, // 删除图元前的处理
        AfterDelete, // 删除图元后的处理
        Edit, // 编辑图元
        CreateLine, // 创建连接线
        CreateAndConnectLine, // 创建连接线并自动连接图元
        CreateChart, // 创建绘图
        AfterLoad, // 绘图加载后
        PopUpContextMenu, // 弹出右键菜单
        ViewCode, // 查看代码
        ViewFolder, // 查看文件夹
        CompileCode, // 生成流程图的代码
        CheckLogic, // 检查流程图的逻辑
        ManageArguments, // 管理流程图的参数
        Save, // 保存流程图
        Paste, // 粘贴图元
    }

    /// <summary>
    /// 游标类型枚举
    /// </summary>
    public enum RodmanType 
    { 
        None, 
        Horizonal, 
        Vertical 
    }
}
