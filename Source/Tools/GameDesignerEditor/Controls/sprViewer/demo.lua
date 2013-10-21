--用于预览spr的字段列表
g_sprFields = {
	"name",
	"mapname"
}


function onselectpropertychanged(prop)
	--其他
	--ProcessPreview(prop)
	
	--spr预览
	if (prop.Value ~= nil and prop.ID ~= nil) then
		for index, value in pairs(g_sprFields) do
			if prop.ID == string.lower(value) then
				spr_show(prop.Value)
				return
			end
		end
	end	
	return
end

function oneditvalue(prop, e)
	trace("b1")
	luanet.load_assembly("sprSelector.dll")
	trace("b2")
	local cls = luanet.import_type("sprSelector.sprViewControl")
	trace("b3")
	local o = cls()
	trace("b4")
	local path = RootDir .. "\\"
	trace("b5")
	local r = o:show(path .. "gamebin\\images\\", "Select spr File:", path .. prop.Value)	
	
	if r == nil then
		return false	
	end	

	if string.len(r) <= string.len(path) + 1 then
		return false
	end
	local r = string.sub(r, string.len(path) + 1 )
	spr_show(r)
	return true, r
end


--show preview window left
function spr_show(filename)
	if filename == nil or filename == "" then
		return
	end
	if _G["g_sprobject"] == nil then
		luanet.load_assembly("sprViewer.dll")
		_G["g_sprobject"] = luanet.import_type("sprViewer.doit")()
		InitPreviewPlugIn(_G["g_sprobject"]:Init())
		trace("spr_show init object")
	end
	trace("Set file: " .. RootDir .. "\\" .. filename)
	_G["g_sprobject"]:SetFileName(RootDir .. "\\" .. filename)
end

--显示道具预览窗口
function showItem()
	if _G["g_uitexobject"] == nil then
		if RegComDll(".\\Plugins\\TGA\\GVBOX.OCX") == false then
			trace("OCX 注册失败")
			return false
		end
		luanet.load_assembly("sprViewer.dll")
		_G["g_uitexobject"] = luanet.import_type("sprViewer.doit")()
		InitPreviewPlugIn(_G["g_uitexobject"]:Init())
		trace("Item init object")
	end	
	local p1 = Pages[0].Properties['UiID.IconID.Name'].Value
	local p2 = Pages[0].Properties["Quality"].ComboValues[Pages[0].Properties["Quality"].Value]
	local p3 = Pages[0].Properties['CanStack'].Value == '是'
	local p4 = Pages[0].Properties['MaxDurability'].Value
	local p5 = Pages[0].Properties['Genre'].ComboValues[Pages[0].Properties["Genre"].Value]
	local p6 =  Pages[0].Properties['BindType'].ComboValues[Pages[0].Properties["BindType"].Value]
	local p7 = Pages[0].Properties['MaxExistAmount'].Value
	local p8 = Pages[0].Properties['UiID.IconID.Desc'].Value
	local p9 = Pages[0].Properties['CanTrade'].Value == '是'
	local p10 = Pages[0].Properties['Price'].Value
	local p11 = RootDir .. '\\Ui\\Image\\icon\\' .. Pages[0].Properties['UiID.IconID.IconID.ID.FileName'].Value
	local p12 = Pages[0].Properties['UiID.IconID.IconID.ID.frame'].Value

	_G["g_uitexobject"]:SetItemInfo(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
end


--弹出道具编辑窗口
function oneditvalue(prop, e)
	if descHelper == nil then
		luanet.load_assembly("descHelper.dll")
		local cls1 = luanet.import_type("descHelper.descEditor")
		--static_descHelper = luanet.import_type("descHelper.descEditor")
		descHelper = cls1()
	end
	if iconselector == nil then
		luanet.load_assembly("IconSelector.dll")
		local cls1 = luanet.import_type("IconSelector.Implement")
		iconselector = cls1()
		local os = iconselector
		os:Init(Conn, RootDir..'\\Ui\\Image\\icon')
	end
    local o = descHelper
    --luanet.descHelper.itemProp.iName = "xx"
	trace(luanet.descHelper.itemProp.iName .. "...ok")
	luanet.descHelper.itemProp.iName 		= Pages[0].Properties['UiID.IconID.Name'].Value
	luanet.descHelper.itemProp.iColor 		= Pages[0].Properties["Quality"].ComboValues[Pages[0].Properties["Quality"].Value]
	luanet.descHelper.itemProp.iCanStack 	= Pages[0].Properties['CanStack'].Value == '是'
	luanet.descHelper.itemProp.iMax 		= Pages[0].Properties['MaxDurability'].Value
	luanet.descHelper.itemProp.tGenre 		= Pages[0].Properties['Genre'].ComboValues[Pages[0].Properties["Genre"].Value]
	luanet.descHelper.itemProp.tBindType 	= Pages[0].Properties['BindType'].ComboValues[Pages[0].Properties["BindType"].Value]
	luanet.descHelper.itemProp.maxExist 	= Pages[0].Properties['MaxExistAmount'].Value
	luanet.descHelper.itemProp.desc 		= Pages[0].Properties['UiID.IconID.Desc'].Value
	luanet.descHelper.itemProp.canTrade 	= Pages[0].Properties['CanTrade'].Value == '是'
	luanet.descHelper.itemProp.price 		= Pages[0].Properties['Price'].Value
	luanet.descHelper.itemProp.icoID 		= Pages[0].Properties['UiID.IconID.IconID'].Value.Value
	luanet.descHelper.itemProp.icoSelector 	= iconselector
	luanet.descHelper.itemProp.conn 		= Conn
	luanet.descHelper.itemProp.iconDir 		= RootDir .. '\\Ui\\Image\\icon'
	local ret = o:showEdit()
	
    if ret then
    	Pages[0].Properties['UiID.IconID.Name'].Value = luanet.descHelper.itemProp.iName
    	Pages[0].Properties['UiID.IconID.Desc'].Value = luanet.descHelper.itemProp.desc
    	Pages[0].Properties['UiID.IconID.IconID'].Value.Value = luanet.descHelper.itemProp.icoID
    	Pages[0].Properties['UiID.IconID.IconID']:SetRealValue(luanet.descHelper.itemProp.icoID)
    	showItem()
    end
    return false
end