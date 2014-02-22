
global Outfile -- 저장될 file
global BinaryOutfile -- 저장될 bynary file
global g_BinaryWrite = 0; -- (No Write=0) (Write=1)
global g_AnimationType = 1	-- (Rigied = 1) (keyanimation = 2) (Skinning = 3)
global g_ExporterType = 0	-- (DX = 0) (OpenGL=1)
global g_nSampling = 1 -- 샘플링 간격
global g_fEpsilon = 0.0001
global g_BmmFilePath
global g_AniFilePath
global g_WriteBmm = true
global g_WriteAni = true
global g_TextureOptimize = true

global g_varSetMaterials = #()
global g_varSetMeshes = #()
global g_varSetBones = #()
global varSetVtxNormTex = #()


-- [MAX quaterion]            [D3D quaternion]
-- x,y,z,w                    -x,-z,-y,w

-- [D3D matrix]               [MAX matrix]
-- _11 _12 _13 0              _11 _13 _12 0
-- _21 _22 _23 0       <->    _31 _33 _32 0
-- _31 _32 _33 0              _21 _23 _22 0
-- _41 _42 _43 1              _41 _43 _42 1

-- Basic type definitions
struct MATRIX_D3D
( 
	_11, _12, _13, _14,
	_21, _22, _23, _24,
	_31, _32, _33, _34,
	_41, _42, _43, _44
)

struct VTXNORMTEX
(
	vtx,
	nrm,
	tex
)

struct PHYSIQDATA
(
	vtxidx,
	varSetVtxWeight
)

struct WEIGHT
(
	BoneName,
	Weight
)


-- binary file에 쓰기위해서 만들어진 타입
struct BINSTRING
(
	size,
	strValue
)
-- binary file에 쓰기위해서 만들어진 타입
struct BINPOINT3
(
	x,y,z
)

struct ANIKEY
(
	start,
	end,
	keypos,
	keyrot,
	keyscale
)






fn TabStep tab = 
(
	for i=1 to tab do
	(
		format "\t" to:Outfile
	)
)


fn ConvertToD3D_POS max_pos =
(
	local	d3d_pos = [0,0,0]

	if g_ExporterType == 0 then
	(
		d3d_pos.x = max_pos.x
		d3d_pos.y = max_pos.z
		d3d_pos.z = max_pos.y
	)
	else
	(
		d3d_pos.x = max_pos.x
		d3d_pos.y = max_pos.z
		d3d_pos.z = -max_pos.y
	)
	
	d3d_pos
)

fn ConvertToD3D_SCALE max_scale =
(
	local	d3d_scale = [0,0,0]

	if g_ExporterType == 0 then
	(
		d3d_scale.x = max_scale.x
		d3d_scale.y = max_scale.z
		d3d_scale.z = max_scale.y
	)
	else
	(
		d3d_scale.x = max_scale.x
		d3d_scale.y = max_scale.z
		d3d_scale.z = max_scale.y
	)
	
	d3d_scale
)


fn ConvertToD3D_INDEX max_index =
(
	local d3d_index = [0,0,0]
	d3d_index.x = max_index.x - 1
	d3d_index.y = max_index.z - 1
	d3d_index.z = max_index.y - 1

	d3d_index
)

fn ConvertToD3D_QUAT max_quat =
(
	local	d3d_quat = quat 0 0 0 0

	if g_ExporterType == 0 then
	(
		d3d_quat.x = -max_quat.x
		d3d_quat.y = -max_quat.z
		d3d_quat.z = -max_quat.y
		d3d_quat.w =  max_quat.w
	)
	else
	(
		d3d_quat.x = -max_quat.x
		d3d_quat.y = -max_quat.z
		d3d_quat.z = max_quat.y
		d3d_quat.w =  max_quat.w
	)
	
	d3d_quat
)

-- 정의 : MAX의 TM을 D3D의 TM으로 바꿔준다.
fn ConvertToD3D_TM max_tm =
(
	local	d3d_tm = MATRIX_D3D	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

	if g_ExporterType == 0 then
	(
		d3d_tm._11 = max_tm.row1.x
		d3d_tm._12 = max_tm.row1.z
		d3d_tm._13 = max_tm.row1.y
		d3d_tm._14 = 0.0

		d3d_tm._21 = max_tm.row3.x
		d3d_tm._22 = max_tm.row3.z
		d3d_tm._23 = max_tm.row3.y
		d3d_tm._24 = 0.0

		d3d_tm._31 = max_tm.row2.x
		d3d_tm._32 = max_tm.row2.z
		d3d_tm._33 = max_tm.row2.y
		d3d_tm._34 = 0.0

		d3d_tm._41 = max_tm.row4.x
		d3d_tm._42 = max_tm.row4.z
		d3d_tm._43 = max_tm.row4.y
		d3d_tm._44 = 1.0
	)
	else
	(
		d3d_tm._11 = max_tm.row1.x
		d3d_tm._12 = max_tm.row1.z
		d3d_tm._13 = -max_tm.row1.y
		d3d_tm._14 = 0.0

		d3d_tm._21 = max_tm.row3.x
		d3d_tm._22 = max_tm.row3.z
		d3d_tm._23 = -max_tm.row3.y
		d3d_tm._24 = 0.0

		d3d_tm._31 = max_tm.row2.x
		d3d_tm._32 = max_tm.row2.z
		d3d_tm._33 = -max_tm.row2.y
		d3d_tm._34 = 0.0

		d3d_tm._41 = max_tm.row4.x
		d3d_tm._42 = max_tm.row4.z
		d3d_tm._43 = -max_tm.row4.y
		d3d_tm._44 = 1.0
	)

	d3d_tm
)

-- 정의 : MAX의 텍스쳐 좌표를 D3D의 uv좌표로 바꿔준다.
fn ConvertToD3D_UV max_pos =
(
	if max_pos == undefined then return undefined
	local d3d_uv = [0,0]

	if g_ExporterType == 0 then
	(
		d3d_uv.x = max_pos.x
		d3d_uv.y = 1.0 - max_pos.y
	)
	else
	(
		d3d_uv.x = max_pos.x
		d3d_uv.y = max_pos.y
	)

	d3d_uv
)


fn FindObject name =
(
	for o in objects do
	(
		if o.name == name then return o
	)
	
	undefined
)


-- 정의 : Biped나 IK사용시 키값을 추출하기 위해서 TM샘플링을 사용하고 있다.
--        이때, snapshot으로 임시 오브젝트를 생성하여 TM값을 샘플링(Sampling or Dump)한다.
fn DumpObjects obj =
(
	origbiped = #()
	snapset = #()

	format "DumpObjects Root : %...\n" obj.name
	a = "$'" + obj.name + "'"
	select (execute (a + "...*"))
	selectionSets ["OrigBiped"] = selection

	for i in selection do 
	(
	    append origbiped i
	    b = snapshot i name:(i.name as string + "_snap")
	    b.parent = undefined
	    b.transform = i.transform
	    b.position.track = bezier_position()
	    b.rotation.track = tcb_rotation()
	    append snapset b
	)
	selectionSets ["Snapset"] = Snapset

	for i in 1 to snapset.count do
	(
	    try
	    ( 
			snapset[i].parent = execute ("$'" + origbiped[i].parent.name + "_snap'")
			snapset[i].position.track = bezier_position()
	   	   	snapset[i].rotation.track = tcb_rotation()
	    )
	    catch()
	)

	animate on
	undo off
	for t = animationRange.start to animationRange.end by g_nSampling do at time t
	(
	    for i in 1 to Snapset.count do
	    (
	        snapset[i].transform = origbiped[i].transform
	    )
	)
	
	snapset
)





-- 정의 : Hierarchy를 고려한 Local Transform Matrix(TM)를 리턴한다.
fn GetLocalTM obj =
(
	local matLocal = copy obj.transform
	if obj.parent != undefined then
	(
		matLocal = obj.transform * (inverse obj.parent.transform)
	)

	matLocal
)

fn GetMeshCount = 
(
	count = 0
	for i=1 to objects.count do
	(
--		if ((classof objects[i]) != BoneGeometry) and ((classof objects[i]) != Biped_object) then
		if (0 == IsBone(objects[i])) then
			count = count + 1
	)
	count
)

fn GetBoneCount = 
(
	count = 0
	for i=1 to objects.count do
	(
--		if ((classof objects[i]) == BoneGeometry) or ((classof objects[i]) == Biped_object) then
		if (1 == IsBone(objects[i])) then
			count = count + 1
	)
	count
)

fn GetAnimationCount = 
(
	count = 0
	print objects.count
	for i=1 to objects.count do
	(
		if (objects[i].pos.isAnimated) or (objects[i].rotation.isAnimated) or (objects[i].scale.isAnimated) then
		( 
			count = count + 1
		)
	)
	count
)


-- {0: Not Bone} {1: Bone}
fn IsBone obj = 
(
	if (classof obj == BoneGeometry) or (classof obj == Biped_Object) then
	(
		return 1
	)
	else if (classof obj == dummy) then
	(
		if ("dummy_weapon" == obj.name) then
		(
			return 1
		)
	)
	else
	(
		s = substring obj.name 1 3
		if s == "bip" then
		(
			return 1
		)
	)
	return 0
)


fn CollectObject = 
(
	g_varSetBones = #()
	g_varSetMeshes = #()

	for i=1 to objects.count do
	(
		if 1 == (IsBone objects[i]) then
		(
			append g_varSetBones objects[i]
		)
		else if (dummy == (classof objects[i])) then
		(
			-- dummy는 처리되지 않는다.
			print "dummy"
			print (objects[i].name)
		)
		else
		(
			print "mesh"
			print (objects[i].name)
			print (classof objects[i])
			append g_varSetMeshes objects[i]
		)
	)
)


fn CollectAnimation obj = 
(
	if (not obj.pos.isAnimated) and (not obj.rotation.isAnimated) and (not obj.scale.isAnimated) then return undefined

	ani = ANIKEY animationRange.start animationRange.end #() #() #()

	if obj.pos.isAnimated then
	(
		obj.pos.controller = tcb_position()
		local keys = obj.pos.controller.keys
		for i=1 to keys.count do
		(
			append ani.keypos keys[i].time
			append ani.keypos (ConvertToD3D_POS keys[i].value)
		)
	)

	if obj.rotation.isAnimated then
	(
		obj.rotation.controller = tcb_rotation()
		local keys = obj.rotation.controller.keys

		local multiquat = quat 0 0 0 1
		for i=1 to keys.count do
		(
			local k = keys[i].value
			if i == 1 then
			(
				k = (quat 0 0 0 1)
			)
			multiquat = multiquat * k
			q = ConvertToD3D_QUAT multiquat

			append ani.keyrot keys[i].time								 
			append ani.keyrot q
		)
	)

	if obj.scale.isAnimated then
	(
		obj.scale.controller = tcb_scale()
		local keys = obj.scale.controller.keys

		for i=1 to keys.count do
		(
			append ani.keyscale keys[i].time
			append ani.keyscale (ConvertToD3D_POS keys[i].value)
		)
	)

	ani
)


fn GetBoneId name =
(
	for i=1 to g_varSetBones.count do
	(
		if name == g_varSetBones[i].name then
			return i as integer
	)
	return 0
)


fn GetMeshId name =
(
	for i=1 to g_varSetMeshes.count do
	(
		if name == g_varSetMeshes[i].name then
			return i as integer
	)
	return 0
)


fn GetObjectId obj = 
(
	if obj == undefined then return 0

	id = 0

--	if (classof obj == BoneGeometry) or (classof obj == Biped_object) then
	if ( 1 == IsBone(obj) ) then
	(
		id = (GetBoneId obj.name)
	)
	else
	(
		id = (GetMeshId obj.name)
	)

	id
)


fn GetParentObjectId obj = 
(
	if obj.parent == undefined then return 0

--	if (classof obj == BoneGeometry) or (classof obj == Biped_object) then
	if ( 1 == IsBone(obj) ) then
	(
		return (GetBoneId obj.parent.name)
	)
	else
	(
		return (GetMeshId obj.parent.name)
	)

	return 0
)


fn CollectMaterial = 
(
	temp = standard()
--	SafeAppend g_varSetMaterials temp
	Append g_varSetMaterials temp

	for i=1 to objects.count do
	(
		if objects[i].material == undefined do continue

		if classof objects[i].material == standardmaterial then
		(
--			SafeAppend g_varSetMaterials objects[i].material
			Append g_varSetMaterials objects[i].material
		)

		if classof objects[i].material == multimaterial then
		(
			for j=1 to objects[i].material.count do
			(
--				SafeAppend g_varSetMaterials objects[i].material[j]
				Append g_varSetMaterials objects[i].material[j]
			)
		)
	)

	varSetMaterials
)


fn CollectVertex obj = 
(
	varSetVertex= #()
	for i=1 to obj.numverts do
	(
		local v = getvert obj i
--		t = v * (inverse obj.transform)
--		append varSetVertex t
		append varSetVertex v
	)
	varSetVertex
)


fn CollectVertexNormTex obj = 
(
	varSetVtxNormTex = #()

	print obj.name
	print (classof obj)
	print "vertex count: " 
	print obj.numverts
	print "texture vertex count: " 
	print obj.numtverts
	print "vertex face count: "
	print obj.faces.count

	if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
	(
		for i=1 to obj.numTVerts do
		(
			local vnt = VTXNORMTEX #()
			vnt.tex = getTvert obj i
			append varSetVtxNormTex vnt
		)

		for i=1 to obj.faces.count do
		(
			local tvfaceidx = getTVFace obj i
			local faceidx = getFace obj i

			varSetVtxNormTex[ tvfaceidx[ 1]].vtx = copy (getvert obj faceidx[ 1])
			varSetVtxNormTex[ tvfaceidx[ 1]].nrm = getnormal obj faceidx[ 1]

			varSetVtxNormTex[ tvfaceidx[ 2]].vtx = copy (getvert obj faceidx[ 2])
			varSetVtxNormTex[ tvfaceidx[ 2]].nrm = getnormal obj faceidx[ 2]

			varSetVtxNormTex[ tvfaceidx[ 3]].vtx = copy (getvert obj faceidx[ 3])
			varSetVtxNormTex[ tvfaceidx[ 3]].nrm = getnormal obj faceidx[ 3]
		)


		-- test code

		test = #()
		for i=1 to obj.numTVerts do
		(
			append test false
		)

		for i=1 to obj.faces.count do
		(
			local tvfaceidx = getTVFace obj i
			test[ tvfaceidx[ 1]] = true
			test[ tvfaceidx[ 2]] = true
			test[ tvfaceidx[ 3]] = true
		)

		for i=1 to obj.numTVerts do
		(
			if test[ i] == false then
			(
				print "missing"
				print i
			)
		)

		for i=1 to obj.faces.count do
		(
			local tvfaceidx = getTVFace obj i
			v1 = varSetVtxNormTex[ tvfaceidx[ 1]].vtx
			v2 = varSetVtxNormTex[ tvfaceidx[ 2]].vtx
			v3 = varSetVtxNormTex[ tvfaceidx[ 3]].vtx

			nrm = (v2 - v1) * (v3 - v1)

	--		varSetVtxNormTex[ tvfaceidx[ 1]].nrm = nrm
	--		varSetVtxNormTex[ tvfaceidx[ 2]].nrm = nrm
	--		varSetVtxNormTex[ tvfaceidx[ 3]].nrm = nrm
		)

	)
	else
	(
		for i=1 to obj.numverts do
		(
			local vnt = VTXNORMTEX #()
			vnt.vtx = getvert obj i
			vnt.nrm = getnormal obj i
			vnt.tex = getTVert obj i
			append varSetVtxNormTex vnt
		)
	)
	
	varSetVtxNormTex
)


fn CollectNormal obj = 
(
	varSetNormal = #()

	for i=1 to obj.numverts do
	(
		local v = getnormal obj i
		append varSetNormal v
	)

	varSetNormal
)


fn CollectTex obj = 
(
	varSetTex = #()

	for i=1 to obj.numTVerts do
	(
		local t = getTVert obj i
		append varSetTex t
	)

	varSetTex
)


fn CollectIndex obj = 
(
	varSetIndex = #()

	if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
	(
		for i=1 to obj.numfaces do
		(
			local v = copy (getTVFace obj i)
			append varSetIndex v
		)
	)
	else
	(
		for i=1 to obj.numfaces do
		(
			local v = copy (getface obj i)
			append varSetIndex v
		)
	)

	varSetIndex
)


fn CollectVertexWeigthBySkin obj = 
(
	print " ---------- CollectVertexWeigthBySkin ----------- "
--	if classof obj != Editable_mesh then return undefined

	unfreeze obj

	local sm
	max modify mode
	sm = obj.modifiers[#skin]
	if sm == undefined then return undefined

	modPanel.setCurrentObject sm
	if obj.numverts == 0 then return undefined

	local nBoneCount = SkinOps.GetNumberBones sm
	local nVertCount = SkinOps.GetNumberVertices sm
	if nVertCount == 0 then return undefined

	setVarPhysique = #()

	for i=1 to nVertCount do
	(
		local phys = PHYSIQDATA #()
		local varSW = #()

		local nVertWeightCount = SkinOps.GetVertexWeightCount sm i
		if 0 < nVertWeightCount then
		(
			for k=1 to nVertWeightCount do
			(
				local vw = WEIGHT 0 0
				nBoneID = (SkinOps.GetVertexWeightBoneID sm i k)
				vw.BoneName = (SkinOps.GetBoneName sm nBoneID 0)
				vw.Weight = (SkinOps.GetVertexWeight sm i k)

				if 0 == (GetBoneId vw.BoneName) then
				(
					continue
				)

				append varSW vw
			)

			phys.vtxidx = i
			phys.varSetVtxWeight = varSW
			append setVarPhysique phys
		)
	)

	setVarRet = #()

	undo on
	(
		ConvertToMesh obj

		if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
		(
			for i=1 to obj.numTVerts do
			(
				local phys = PHYSIQDATA #()
				append setVarRet phys
			)

			for i=1 to obj.faces.count do
			(
				local tvfaceidx = getTVFace obj i
				local faceidx = getFace obj i
				
				phys1 = copy (setVarPhysique[ faceidx[ 1]])
				phys1.vtxidx = tvfaceidx[ 1]
				setVarRet[ tvfaceidx[ 1]] = phys1

				phys2 = copy (setVarPhysique[ faceidx[ 2]])
				phys2.vtxidx = tvfaceidx[ 2]
				setVarRet[ tvfaceidx[ 2]] = phys2

				phys3 = copy (setVarPhysique[ faceidx[ 3]])
				phys3.vtxidx = tvfaceidx[ 3]
				setVarRet[ tvfaceidx[ 3]] = phys3
			)
		)
		else
		(
			setVarRet = setVarPhysique
		)
	)
	max undo

	setVarRet
)


fn CollectVertexWeightByPhysique obj = 
(
	print " ---------- CollectVertexWeightByPhysique ----------- "	
	if classof obj != Editable_mesh then return undefined
	print obj
		

--	local pm
--	max modify mode
--	pm = physiqueOps.getPhysiqueModifier obj
--	if pm == undefined then return undefined
--	bones = physiqueOps.getBones obj modifier:pm
--	if bones == undefined then return undefined
	
	nvpx.PhysiqueLoad obj

--	nVertCount = physiqueOps.getVertexCount obj modifier:pm
	nBoneCount = nvpx.PhysiqueGetNumBones obj 
--	if bones != undefined then nBoneCount = bones.count
--	else return undefined
	
	format "nBoneCount = % \n" nBoneCount

	if nBoneCount == 0 then return undefined

--	ow.nBoneCount = nBoneCount

	setVarPhysique = #()

	for i = 1 to nBoneCount do
	(
		local phys = PHYSIQDATA #()
		local varSW = #()

--		local nVertWeightCount = physiqueOps.getVertexBoneCount obj i modifier:pm
		local bone0 = nvpx.PhysiqueGetBone obj i
		local nVertWeightCount = nvpx.PhysiqueGetNumBoneVertices obj bone
		
		if 0 < nVertWeightCount then
		(
			for k = 1 to nVertWeightCount do 
			(
				local vw = WEIGHT 0 0
--				vw.BoneName = (physiqueOps.getVertexBone obj i k modifier:pm).name
				vw.BoneName = bone0.name
				--vw.Weight = (physiqueOps.getVertexWeight obj i k modifier:pm)
				local points = nvpx.PhysiqueGetBoneVertex obj bone0 k
				
				vw.Weight = 0

				append varSW vw
			)

			phys.vtxidx = i
			phys.varSetVtxWeight = varSW
			append setVarPhysique phys
		)
	) 

	setVarRet = #()

	
	undo on
	(
		ConvertToMesh obj

		if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
		(
			for i=1 to obj.numTVerts do
			(
				local phys = PHYSIQDATA #()
				append setVarRet phys
			)

			for i=1 to obj.faces.count do
			(
				local tvfaceidx = getTVFace obj i
				local faceidx = getFace obj i
				
				phys1 = copy (setVarPhysique[ faceidx[ 1]])
				phys1.vtxidx = tvfaceidx[ 1]
				setVarRet[ tvfaceidx[ 1]] = phys1

				phys2 = copy (setVarPhysique[ faceidx[ 2]])
				phys2.vtxidx = tvfaceidx[ 2]
				setVarRet[ tvfaceidx[ 2]] = phys2

				phys3 = copy (setVarPhysique[ faceidx[ 3]])
				phys3.vtxidx = tvfaceidx[ 3]
				setVarRet[ tvfaceidx[ 3]] = phys3
			)
		)
		else
		(
			setVarRet = setVarPhysique
		)
	)
	max undo

	setVarRet
)


fn GetObjectMaterialId obj =
(
	if classOf obj.material == UndefinedClass then
	(
		return 0
	)
	
	if classOf obj.material == Standardmaterial then
	(
		for i=1 to g_varSetMaterials.count do
		(
			if obj.material == g_varSetMaterials[i] then
			(
				return i as integer
			)
		)
	)
	return 0
)


fn WriteBinaryFile_Matrix tm=
(
	WriteFloat BinaryOutfile tm._11
	WriteFloat BinaryOutfile tm._12
	WriteFloat BinaryOutfile tm._13
	WriteFloat BinaryOutfile tm._14
	WriteFloat BinaryOutfile tm._21
	WriteFloat BinaryOutfile tm._22
	WriteFloat BinaryOutfile tm._23
	WriteFloat BinaryOutfile tm._24
	WriteFloat BinaryOutfile tm._31
	WriteFloat BinaryOutfile tm._32
	WriteFloat BinaryOutfile tm._33
	WriteFloat BinaryOutfile tm._34
	WriteFloat BinaryOutfile tm._41
	WriteFloat BinaryOutfile tm._42
	WriteFloat BinaryOutfile tm._43
	WriteFloat BinaryOutfile tm._44
)


fn WriteBFile value = 
(
	case (classof value) of
	(
		Integer: 
		(
			WriteLong BinaryOutfile value
		)
		Float:
		(
			WriteFloat BinaryOutfile value
		)
		Point3:
		(
			WriteFloat BinaryOutfile value.x
			WriteFloat BinaryOutfile value.y
			WriteFloat BinaryOutfile value.z
		)
		Point2:
		(
			WriteFloat BinaryOutfile value.x
			WriteFloat BinaryOutfile value.y
		)
		Time:
		(
			WriteFloat BinaryOutfile value
		)
		Color:
		(
			WriteFloat BinaryOutfile value.r
			WriteFloat BinaryOutfile value.g
			WriteFloat BinaryOutfile value.b
			WriteFloat BinaryOutfile value.a
		)
		Quat:
		(
			WriteFloat BinaryOutfile value.x 
			WriteFloat BinaryOutfile value.y 
			WriteFloat BinaryOutfile value.z 
			WriteFloat BinaryOutfile value.w
		)
		BINSTRING:
		(
			-- null문자까지 쓰여짐
			WriteString BinaryOutfile value.strValue

			if value.strValue.count == value.size then
			(
				f = ftell BinaryOutfile
				fseek BinaryOutfile (-1) #seek_cur
			)
			else
			(
				i = value.strValue.count + 1
				while i < value.size do
				(
					WriteByte BinaryOutfile 0
					i = i + 1
				)
			)
		)
		PHYSIQDATA:
		(
			WriteShort BinaryOutfile (value.vtxidx-1)
			WriteShort BinaryOutfile value.varSetVtxWeight.count
			WriteLong BinaryOutfile 0

			for i=1 to value.varSetVtxWeight.count do
			(
--				WriteLong BinaryOutfile (value.varSetVtxWeight[i].BoneId-1)
--				WriteFloat BinaryOutfile value.varSetVtxWeight[i].Weight
			)
			for i=(value.varSetVtxWeight.count+1) to 4 do
			(
--				WriteLong BinaryOutfile 0
--				WriteFloat BinaryOutfile 0.0
			)
		)
		WEIGHT:
		(
--			WriteLong BinaryOutfile (value.BoneId-1)
			WriteLong BinaryOutfile ((GetBoneId value.BoneName)-1)
			WriteFloat BinaryOutfile value.Weight
		)

		BINPOINT3: 
		(
			WriteShort BinaryOutfile value.x
			WriteShort BinaryOutfile value.y
			WriteShort BinaryOutfile value.z
		)
		MATRIX_D3D: 
		(
			print "MATRIX_D3D"
			WriteBinaryFile_Matrix value
		)
	)
)


fn WriteBFileString value size =
(
	str = BINSTRING size value 
	WriteBFile str
)


fn WriteInfo tab =
(
--	TabStep tab;	format "OBJECT_COUNT %\n" objects.count to:Outfile

	g_AnimationType = 1
	for i=1 to objects.count do
	(
--		if (classof objects[i] == BoneGeometry) or (classof objects[i] == Biped_Object)	then
		if ( 1 == IsBone(objects[i]) ) then
		(
			g_AnimationType = 3
			break;
		)
	)

	-- keyanimation 인지 검사
	if 3 != g_AnimationType then
	(
		for i=1 to objects.count do
		(
			if objects[i].pos.isAnimated or objects[i].rotation.isAnimated or objects[i].scale.isAnimated then
			(
				g_AnimationType = 2
				break;
			)
		)
	)

	-- (Rigied = 1) (keyanimation = 2) (Skinning = 3)
	TabStep tab;	format "OBJECT_TYPE type=%\n" g_AnimationType to:Outfile

)


fn WriteMeshInfo obj tab = 
(
	TabStep tab;
	format "MESH id=% name=\"%\" parentId=% materialId=%\n" ((GetObjectId obj)-1) obj.name ((GetParentObjectId obj)-1) ((GetObjectMaterialId obj)-1) to:Outfile
)


fn WriteBoneInfo obj tab = 
(
	print "##### WriteBoneInfo ####"
	
	TabStep tab;
	format "BONE id=% name=\"%\" parentId=%\n" ((GetObjectId obj)-1) obj.name ((GetParentObjectId obj)-1) to:Outfile
)


fn WriteBinaryFile_ObjectInfo objid obj = 
(
	-- id
	WriteBFile (objid-1)
	-- name
	WriteBFileString obj.name 32
	-- parent id
	WriteBFile ((GetParentObjectId obj)-1)
	-- mtrlid
	WriteBFile (GetObjectMaterialId obj-1)

)

fn WriteBinaryFile_BoneInfo obj = 
(
	-- id
	WriteBFile ((GetObjectId obj)-1)
	-- name
	WriteBFileString obj.name 32
	-- parent id
	WriteBFile ((GetParentObjectId obj)-1)
)


fn WriteVertex obj tab = 
(
	if (classof obj) == BoneGeometry then 
	(
		return undefined
	)
	if (classof obj) == Biped_Object then 
	(
		return undefined
	)
	if (classof obj) == dummy then 
	(
		return undefined
	)

--	undo on
	(
--		ConvertToMesh obj

		varSetVertice = CollectVertexNormTex obj
		varSetTex	  = CollectTex obj

		if 0 < varSetTex.count then
		(
			TabStep tab;	format "VERTEXNORMAL count=0\n"  to:Outfile
			TabStep tab;	format "{\n" to:Outfile
			TabStep tab;	format "}\n" to:Outfile

			TabStep tab;	format "VERTEXNORMALTEX count=%\n" varSetVertice.count to:Outfile
			TabStep tab;	format "{\n" to:Outfile
			for i=1 to varSetVertice.count do
			(
				vtx = (ConvertToD3D_POS varSetVertice[ i].vtx)
				nrm = (ConvertToD3D_POS varSetVertice[ i].nrm)
				tex = (ConvertToD3D_UV varSetVertice[i].tex)
				
				--TabStep (tab+1);	format "VECNORMTEX % % %\n" (ConvertToD3D_POS varSetVertice[ i].vtx) (ConvertToD3D_POS varSetVertice[ i].nrm)  (ConvertToD3D_UV varSetVertice[i].tex) to:Outfile
				TabStep (tab+1);	
				format "VECNORMTEX [x=%,y=%,z=%] [nx=%,ny=%,nz=%] [u=%,v=%] \n" vtx.x vtx.y vtx.z nrm.x nrm.y nrm.z tex.x tex.y to:Outfile
			)
			TabStep tab;	format "}\n" to:Outfile
		)
		else
		(
			TabStep tab;	format "VERTEXNORMAL count=%\n" varSetVertice.count to:Outfile
			TabStep tab;	format "{\n" to:Outfile
			for i=1 to varSetVertice.count do
			(
				vtx = (ConvertToD3D_POS varSetVertice[ i].vtx) 
				nrm = (ConvertToD3D_POS varSetVertice[ i].nrm)
				--TabStep (tab+1);	format "VECNORM % %\n" (ConvertToD3D_POS varSetVertice[ i].vtx) (ConvertToD3D_POS varSetVertice[ i].nrm)  to:Outfile
				TabStep (tab+1);	format "VECNORM [x=%,y=%,z=%] [nx=%,ny=%,nz=%]\n" vtx.x vtx.y vtx.z nrm.x nrm.y nrm.z to:Outfile
			)
			TabStep tab;	format "}\n" to:Outfile

			TabStep tab;	format "VERTEXNORMALTEX 0\n" to:Outfile
			TabStep tab;	format "{\n" to:Outfile
			TabStep tab;	format "}\n" to:Outfile
		)
	)
--	max undo
)

fn WriteBinaryFile_VertexInfo obj = 
(
	-- rigied Animation 이라면
	if( 1 == g_AnimationType ) then
	(
		ConvertToMesh obj
	)

	if 0 < obj.numTVerts then
	(
		WriteBFile 0 -- VertexNorm size = 0 
		WriteBFile 0 -- pointer 초기화
--		WriteBFile obj.numTVerts
		WriteBFile obj.numverts
		WriteBFile 0 -- pointer 초기화
	)
	else
	(
		WriteBFile obj.numverts
		WriteBFile 0 -- pointer 초기화
		WriteBFile 0 -- VertexNormTex size = 0
		WriteBFile 0 -- pointer 초기화
	)

)

fn WriteBinaryFile_Vertex obj = 
(
	if classof obj != Editable_mesh then 
	(
		return undefined
	)

	-- rigied Animation 이라면
	if( 1 == g_AnimationType ) then
	(
		ConvertToMesh obj
	)

	varSetVertice = CollectVertex obj
	varSetNormals = CollectNormal obj
	varSetTex	  = CollectTex obj

	if 0 < varSetTex.count then
	(
		for i=1 to varSetVertice.count do
		(
			WriteBFile (ConvertToD3D_POS varSetVertice[ i])
			WriteBFile (ConvertToD3D_POS varSetNormals[ i])
			WriteBFile (ConvertToD3D_UV varSetTex[i])
		)
	)
	else
	(
		for i=1 to varSetVertice.count do
		(
			WriteBFile (ConvertToD3D_POS varSetVertice[ i])
			WriteBFile (ConvertToD3D_POS varSetNormals[ i])
		)
	)
)


fn WriteIndex obj tab = 
(
	if classof obj != Editable_mesh then return undefined

	varSetIndex = CollectIndex obj

	TabStep tab;	format "INDEX count=%\n" varSetIndex.count to:Outfile
	TabStep tab;	format "{\n" to:Outfile

	for i=1 to varSetIndex.count do
	(
		index = (ConvertToD3D_INDEX varSetIndex[ i])
		--TabStep (tab+1);	format "IDX %\n" (ConvertToD3D_INDEX varSetIndex[ i]) to:Outfile
		TabStep (tab+1);	format "IDX [x=%,y=%,z=%]\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
	)

	TabStep tab;	format "}\n" to:Outfile
)

fn WriteBinaryFile_IndexInfo obj = 
(
	if classof obj != Editable_mesh then return undefined

	WriteBFile obj.numfaces
	WriteBFile 0 -- pointer 초기화
)

fn WriteBinaryFile_Index obj = 
(
	if classof obj != Editable_mesh then return undefined

	varSetIndex = CollectIndex obj

	for i=1 to varSetIndex.count do
	(
		cvtidx = (ConvertToD3D_INDEX varSetIndex[ i])
		idx = BINPOINT3 cvtidx.x cvtidx.y cvtidx.z
		WriteBFile idx
	)
)


fn WriteMaterial tab = 
(
	TabStep tab;	format "MATERIALGROUP count=%\n" g_varSetMaterials.count to:Outfile
	TabStep tab;	format "{\n" to:Outfile
	tab = tab + 1

	for i=1 to g_varSetMaterials.count do
	(
		TabStep tab;	format "MATERIAL id=%\n" (i-1) to:Outfile
		TabStep tab;	format "{\n" to:Outfile

		TabStep (tab+1);	format "DIFFUSE [x=% y=% z=% a=1]\n"  (g_varSetMaterials[i].Diffuse.r/255) (g_varSetMaterials[i].Diffuse.g/255) (g_varSetMaterials[i].Diffuse.b/255) to:Outfile
		TabStep (tab+1);	format "AMBIENT [x=% y=% z=% a=1]\n"  (g_varSetMaterials[i].Ambient.r/255) (g_varSetMaterials[i].Ambient.g/255) (g_varSetMaterials[i].Ambient.b/255) to:Outfile
		TabStep (tab+1);	format "SPECULAR [x=% y=% z=% a=1]\n" (g_varSetMaterials[i].Specular.r/255) (g_varSetMaterials[i].Specular.g/255) (g_varSetMaterials[i].Specular.b/255) to:Outfile
		TabStep (tab+1);	format "EMISSIVE [x=% y=% z=% a=0]\n" (g_varSetMaterials[i].Diffuse.r/255) (g_varSetMaterials[i].Diffuse.g/255) (g_varSetMaterials[i].Diffuse.b/255) to:Outfile
--		TabStep (tab+1);	format "EMISSIVE [0 0 0 1]\n"  to:Outfile
		TabStep (tab+1);	format "POWER val=%\n"  g_varSetMaterials[i].SpecularLevel to:Outfile

		-- Diffuse Map
		if g_varSetMaterials[i].DiffuseMap != undefined then
		(
			TabStep (tab+1);format "TEXTURE \"name=%\"\n" g_varSetMaterials[i].DiffuseMap.filename to:Outfile
		)

		TabStep tab;	format "}\n" to:Outfile
	)

	TabStep (tab-1);	format "}\n" to:Outfile
)


fn WriteBinaryFile_MaterialInfo obj = 
(
	WriteBFile 0

)


fn WriteBinaryFile_Material = 
(
	for i=1 to g_varSetMaterials.count do
	(
		WriteBFile (i-1)
		WriteBFile 1.0; WriteBFile 1.0; WriteBFile 1.0; WriteBFile 0.0
		WriteBFile 1.0; WriteBFile 1.0; WriteBFile 1.0; WriteBFile 0.0
		WriteBFile g_varSetMaterials[i].Specular;
		WriteBFile 1.0; WriteBFile 1.0; WriteBFile 1.0; WriteBFile 0.0
		WriteBFile g_varSetMaterials[i].SpecularLevel

		-- Diffuse Map
		if g_varSetMaterials[i].DiffuseMap != undefined then
		(
			WriteBFileString g_varSetMaterials[i].DiffuseMap.filename 64
		)
		else
		(
			WriteBFileString "" 64
		)
	)
)


fn WriteKey obj objid tab = 
(
	if (not obj.pos.isAnimated) and (not obj.rotation.isAnimated) and (not obj.scale.isAnimated) then return undefined

	postrack = #()
	rottrack = #()
	scaletrack = #()

	if obj.pos.isAnimated then
	(
		obj.pos.controller = tcb_position()
		local keys = obj.pos.controller.keys
		local oldkey

		start = 1
		if keys.count > 1 then	start = 2

		for i=start to keys.count do
		(
			-- 전의 키와 같다면 넘어간다.
			if i > start then
			(
				if abs (oldkey.x - keys[i].value.x) < g_fEpsilon and
				   abs (oldkey.y - keys[i].value.y) < g_fEpsilon and
				   abs (oldkey.z - keys[i].value.z) < g_fEpsilon do continue
			)
			oldkey = keys[i].value
			append postrack keys[i]
		)
	)
	else
	(
		-- 키가 없어도 샘플링 한 데이타에서 첫번째 키를 뽑는다. 
		-- (에니메이션이 있다면 모든 성분(pos,rot,scale)의 값을 뽑기위해.)
		-- append postrack obj.pos.controller.keys[1]
	)

	if obj.rotation.isAnimated then
	(
		obj.rotation.controller = tcb_rotation()
		local keys = obj.rotation.controller.keys

		for i=1 to keys.count do
		(
			-- 단위 쿼터니온이면 넘어간다.
			local k = keys[i].value as quat
			if i > 1 then
			(
				if abs (1 - k.w) < g_fEpsilon and
				   abs (0 - k.x) < g_fEpsilon and
				   abs (0 - k.y) < g_fEpsilon and
				   abs (0 - k.z) < g_fEpsilon do continue
			)
			append rottrack keys[i]
		)
	)
	else
	(
		-- 키가 없어도 샘플링 한 데이타에서 0번 키를 뽑는다.
		-- append rottrack obj.rotation.controller.keys[1]
	)

	if obj.scale.isAnimated then
	(
		obj.scale.controller = tcb_scale()
		local keys = obj.scale.controller.keys
		local oldkey

		start = 1
		if keys.count > 1 then	start = 2

		for i=start to keys.count do
		(
			-- 전의 키와 같다면 넘어간다.
			if i > start then
			(
				if abs (oldkey.x - keys[i].value.x) < g_fEpsilon and
				   abs (oldkey.y - keys[i].value.y) < g_fEpsilon and
				   abs (oldkey.z - keys[i].value.z) < g_fEpsilon do continue
			)
			oldkey = keys[i].value
			append scaletrack keys[i]
		)
	)
	else
	(
		-- 키가 없어도 샘플링 한 데이타에서 0번 키를 뽑는다.
		-- append scaletrack obj.scale.controller.keys[1]
	)

	-- File에 쓴다.
	TabStep tab;	format "ANIMATION id=% startT=% endT=%\n" objid animationRange.start animationRange.end to:Outfile
	TabStep tab;	format "{\n" to:Outfile
	tab = tab + 1

		-- Write Position
		TabStep tab;	format "KEY_POS count=%\n" postrack.count to:Outfile
		TabStep tab;	format "{\n" to:Outfile
			for i = 1 to postrack.count do
			(
				local t = postrack[i].time
				if t < 0 then t = 0
				pos = (ConvertToD3D_POS postrack[i].value)
--				TabStep (tab+1); format "FRAME_POS % % \n" t (ConvertToD3D_POS postrack[i].value) to:Outfile
				TabStep (tab+1); format "FRAME_POS t=% [x=%,y=%,z=%] \n" t pos.x pos.y pos.z to:Outfile				
			)
		TabStep tab;	format "}\n" to:Outfile	

		-- Write Rotation
		rotcount = rottrack.count
		if rottrack.count > 1 then rotcount = rotcount - 1

		TabStep tab;	format "KEY_ROT count=%\n" rotcount to:Outfile
		TabStep tab;	format "{\n" to:Outfile
			local multiquat = quat 0 0 0 1
			for i = 1 to rottrack.count do
			(
				-- 단위 쿼터니온이면 넘어간다.
				local k = rottrack[i].value as quat
				if i == 1 then k = (quat 0 0 0 1) 
				multiquat = k * multiquat

				local t = rottrack[i].time
				if t < 0 then t = 0
				q = ConvertToD3D_QUAT multiquat

				-- rotcout가 1보다 크다면 첫번째 프레임은 스킵된다.
				if ((rottrack.count > 1) and (i > 1)) then
				(
					TabStep (tab+1); format "FRAME_ROT t=% [x=% y=% z=% w=%]\n" t q.x q.y q.z q.w to:Outfile
				)
				if (rottrack.count == 1) then
				(
					TabStep (tab+1); format "FRAME_ROT t=% [x=% y=% z=% w=%]\n" t q.x q.y q.z q.w to:Outfile
				)
			)
		TabStep tab;	format "}\n" to:Outfile	

		-- Write Scaling
		TabStep tab;	format "KEY_SCALE count=%\n" scaletrack.count to:Outfile
		TabStep tab;	format "{\n" to:Outfile
			for i = 1 to scaletrack.count do
			(
				local t = scaletrack[i].time
				if t < 0 then t = 0
				scale = (ConvertToD3D_SCALE scaletrack[i].value)
--				TabStep (tab+1); format "FRAME_SCALE % % \n" t (ConvertToD3D_SCALE scaletrack[i].value) to:Outfile
				TabStep (tab+1); format "FRAME_SCALE t=% [x=%, y=%, z=%] \n" t scale.x scale.y scale.z to:Outfile				
			)
		TabStep tab;	format "}\n" to:Outfile	

	TabStep (tab-1);	format "}\n" to:Outfile
)


fn WriteBinaryFile_KeyInfo objid obj = 
(
	if (not obj.pos.isAnimated) and (not obj.rotation.isAnimated) and (not obj.scale.isAnimated) then return undefined

	varSetAni = CollectAnimation obj

	-- frame, key값이 같이있기때문에 전체갯수는 /2 해줘야한다

	WriteBFile (objid-1)
	WriteBFile varSetAni.start -- animationRange.start
	WriteBFile varSetAni.end -- animationRange.end
	WriteBFile ((varSetAni.keypos.count)/2)
	WriteBFile 0
	WriteBFile ((varSetAni.keyrot.count)/2)
	WriteBFile 0
	WriteBFile ((varSetAni.keyscale.count)/2)
	WriteBFile 0
)


fn WriteBinaryFile_Key objid obj = 
(
	varSetAni = CollectAnimation obj

	if varSetAni == undefined then
	(
		return undefined
	)

	for i=1 to varSetAni.keypos.count do
	(
		WriteBFile varSetAni.keypos[ i]
	)
	for i=1 to varSetAni.keyrot.count do
	(
		WriteBFile varSetAni.keyrot[ i]
	)
	for i=1 to varSetAni.keyscale.count do
	(
		WriteBFile varSetAni.keyscale[ i]
	)

)


fn WriteTM obj tab = 
(
	TabStep tab;	format "LOCALTM \n" to:Outfile
	TabStep tab;	format "{\n" to:Outfile
	tm = (ConvertToD3D_TM (GetLocalTM obj) )
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._11 tm._12 tm._13 tm._14 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._21 tm._22 tm._23 tm._24 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._31 tm._32 tm._33 tm._34 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._41 tm._42 tm._43 tm._44 to:Outfile
	TabStep tab;	format "}\n" to:Outfile

	TabStep tab;	format "WORLDTM \n" to:Outfile
	TabStep tab;	format "{\n" to:Outfile
	tm = (ConvertToD3D_TM (obj.transform) )
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._11 tm._12 tm._13 tm._14 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._21 tm._22 tm._23 tm._24 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._31 tm._32 tm._33 tm._34 to:Outfile
	TabStep (tab+1);	format "MAT [x=%,y=%,z=%,w=%]\n" tm._41 tm._42 tm._43 tm._44 to:Outfile
	TabStep tab;	format "}\n" to:Outfile
)

fn WriteBinaryFile_TM obj = 
(
	WriteBFile (ConvertToD3D_TM (GetLocalTM obj))
	WriteBFile (ConvertToD3D_TM (obj.transform))
)



fn WriteVertexWeight obj tab = 
(
	print "---------------------- WriteVertexWeight --------------------"
	print  obj.name
	print  obj.modifiers[#skin]
	print  obj.modifiers[#physique]
	ret = undefined

	if obj.modifiers[#skin] != undefined then ret = (CollectVertexWeigthBySkin obj)
	if obj.modifiers[#physique] != undefined then ret = (CollectVertexWeightByPhysique obj)

	if ret == undefined then
	(
		print "######## no weight ############# "
		TabStep tab;	format "VERTEXWEIGHT 0\n" to:Outfile
		TabStep tab;	format "{\n" to:Outfile
		TabStep tab;	format "}\n" to:Outfile
		return undefined
	)

	TabStep tab;	format "VERTEXWEIGHT count=%\n" ret.count to:Outfile
	TabStep tab;	format "{\n" to:Outfile

	for i=1 to ret.count do
	(
		TabStep (tab+1);	format "VTXWEIGHT id=% count=%\n" ((ret[i].vtxidx-1) as integer) (ret[i].varSetVtxWeight.count) to:Outfile
		TabStep (tab+1);	format "{\n" to:Outfile
		for j=1 to ret[i].varSetVtxWeight.count do
		(
--			TabStep (tab+2);	format "WEIGHTINFO % %\n" (ret[i].varSetVtxWeight[j].BoneId-1) (ret[i].varSetVtxWeight[j].Weight) to:Outfile
--			TabStep (tab+2);	format "WEIGHTINFO % %\n" ((GetBoneId ret[i].varSetVtxWeight[j].BoneName)-1) (ret[i].varSetVtxWeight[j].Weight) to:Outfile
			TabStep (tab+2);	format "WEIGHTINFO bone=% w=%\n" ((GetBoneId ret[i].varSetVtxWeight[j].BoneName)-1) (ret[i].varSetVtxWeight[j].Weight) to:Outfile
		)
		TabStep (tab+1);	format "}\n" to:Outfile
	)	

	TabStep tab;	format "}\n" to:Outfile
)


fn WriteBinaryFile_VertexWeightInfo obj = 
(
	if obj.modifiers[#skin] == undefined then
	(
		WriteBFile 0
		WriteBFile 0
	)
	else
	(
		if obj.numverts == 0 then
		(
			WriteBFile 0
			WriteBFile 0
		)
		else if (classof obj) != Editable_mesh then
		(
			WriteBFile 0
			WriteBFile 0
		)
		else
		(
			ret = CollectVertexWeigthBySkin obj

			WriteBFile ret.count
			WriteBFile 0
		)
	)

)


fn WriteBinaryFile_VertexWeight obj = 
(
	ret = undefined

	if obj.modifiers[#skin] != undefined then ret = (CollectVertexWeigthBySkin obj)
--	if obj.modifiers[#physique] != undefined then ret = (CollectVertexWeightByPhysique obj)

	if ret == undefined then
	(
		return undefined
	)

	for i=1 to ret.count do
	(
		WriteBFile ret[i]
	)	
)


fn WriteBinaryFile_VertexWeightData obj = 
(
	ret = undefined

	if obj.modifiers[#skin] != undefined then ret = (CollectVertexWeigthBySkin obj)
--	if obj.modifiers[#physique] != undefined then ret = (CollectVertexWeightByPhysique obj)

	if ret == undefined then
	(
		return undefined
	)

	for i=1 to ret.count do
	(
		for k=1 to ret[i].varSetVtxWeight.count do
		(
			WriteBFile ret[ i].varSetVtxWeight[ k]

--			WriteLong BinaryOutfile (ret[i].varSetVtxWeight[i].BoneId-1)
--			WriteFloat BinaryOutfile value.varSetVtxWeight[i].Weight
		)
	)	
)


fn WriteBinaryFile = 
(
	-- mesh, material 정보를 file에 쓴다. (mmp = mesh, material, physique)
	BinaryOutfile = fopen "mesh.mmp" "wb"
	-- type저장 (Rigied = 1) (keyanimation = 2) (Skinning = 3)
	-- file format
	WriteBFileString "mmp" 3
	-- object type
	WriteBFile g_AnimationType
	-- bonefilename
	if 3 ==  g_AnimationType then
	(
		WriteBFileString "bone.b" 64
	)
	else
	(
		WriteBFileString "" 64
	)

	-- mesh size
	WriteBFile (GetMeshCount())
	-- material size
	WriteBFile g_varSetMaterials.count

	for i=1 to g_varSetMeshes.count do
	(
		WriteBinaryFile_ObjectInfo i g_varSetMeshes[i]
		WriteBinaryFile_VertexInfo g_varSetMeshes[ i]
		WriteBinaryFile_IndexInfo g_varSetMeshes[ i]
		WriteBinaryFile_VertexWeightInfo g_varSetMeshes[ i]
		WriteBinaryFile_MaterialInfo g_varSetMeshes[ i]
	)
	-- vertex,index, physiq 정보를 file에 쓴다.
	for i=1 to g_varSetMeshes.count do
	(
		WriteBinaryFile_Vertex g_varSetMeshes[ i]
		WriteBinaryFile_Index g_varSetMeshes[ i]
		WriteBinaryFile_VertexWeight g_varSetMeshes[ i]
	)
	-- 나머지 physiq 정보 file에 쓴다.
	for i=1 to g_varSetMeshes.count do
	(
		WriteBinaryFile_VertexWeightData g_varSetMeshes[ i]
	)

	-- material
	WriteBinaryFile_Material()
	fclose BinaryOutfile

	-- skin animation
	if 3 == g_AnimationType then
	(
		-- bone, physique 정보를 file에 쓴다. (b = bone)
		BinaryOutfile = fopen "bone.b" "wb"
		-- file format
		WriteBFileString "b" 3
		-- bone size
		WriteBFile (GetBoneCount())

		for i=1 to g_varSetBones.count do
		(
			WriteBinaryFile_BoneInfo g_varSetBones[i]
			WriteBinaryFile_TM g_varSetBones[i] 
		)
		fclose BinaryOutfile


		-- animation 정보를 file에 쓴다. (a = animation)
		BinaryOutfile = fopen "animation.a" "wb"
		-- file format
		WriteBFileString "a" 3
		-- type
		WriteBFile g_AnimationType
		WriteBFile (GetAnimationCount())

		for i=1 to g_varSetBones.count do
		(
			WriteBinaryFile_KeyInfo	i g_varSetBones[i]
		)
		for i=1 to g_varSetBones.count do
		(
			WriteBinaryFile_Key i g_varSetBones[i]
		)

		fclose BinaryOutfile
	)
)


fn ExporterStart = 
(
	CollectObject()
	CollectMaterial()

	if (g_WriteAni == true) or (g_WriteBmm == true) then
	(
		Outfile = createfile g_BmmFilePath

		format "GPJ\n" to:Outfile

		format "BMMFORM\n" to:Outfile
		format "{\n" to:Outfile

			WriteInfo 1
			WriteMaterial 1
		
			TabStep 1;	format "MESHGROUP count=%\n" g_varSetMeshes.count to:Outfile
			TabStep 1;	format "{\n" to:Outfile

			for i=1 to g_varSetMeshes.count do
			(
				print( g_varSetMeshes[i] )
				print( g_varSetMeshes[i].name )
				
				WriteMeshInfo g_varSetMeshes[i] 2
				TabStep 2;	format "{\n" to:Outfile
				
					-- vertex, index값은 EditMesh모드일때만 얻어올수 있어서 ConvertToMesh 함수를
					-- 사용했다. 문제는 EditMesh모드일때 VertexWeight값을 얻어올수 없다는것이다.
					-- 그래서 Vertex, index값을 얻은 후 undo명령어로 이전상태로 바꾼후 얻어온다.
					undo on
					(
						ConvertToMesh g_varSetMeshes[ i]
						WriteVertex g_varSetMeshes[ i] 3
						WriteIndex g_varSetMeshes[ i] 3
					)
					max undo

					WriteVertexWeight g_varSetMeshes[ i] 3

				TabStep 2;	format "}\n" to:Outfile
			)
			
			TabStep 1;	format "}\n" to:Outfile

			TabStep 1;	format "BONEGROUP count=%\n" g_varSetBones.count to:Outfile
			TabStep 1;	format "{\n" to:Outfile
			for i=1 to g_varSetBones.count do
			(
				WriteBoneInfo g_varSetBones[i] 2
				TabStep 2;	format "{\n" to:Outfile
					WriteTM g_varSetBones[ i] 3
				TabStep 2;	format "}\n" to:Outfile
			)
			TabStep 1;	format "}\n" to:Outfile

		format "}\n" to:Outfile

		close Outfile
	)

	if (g_WriteAni == true) then
	(
		-- 에니메이션 정보 저장
		Outfile = createfile g_AniFilePath

		format "GPJ\n" to:Outfile

		TabStep 0;	format "ANIFORM count=1\n" to:Outfile
		TabStep 0;	format "{\n" to:Outfile

		-- Skinning 에니메이션된 모델일때
		if g_AnimationType == 3 then
		(
			-- Bone에 설정된 에니메이션만 뽑아온다.
			TabStep 1;	format "ANIMATIONGROUP id=0 name=\"ani_name\" count=%\n" g_varSetBones.count to:Outfile
			TabStep 1;	format "{\n" to:Outfile
				for i=1 to g_varSetBones.count do
				(
					DumpObjects g_varSetBones[ i] 
					snapobj = FindObject (g_varSetBones[ i].name + "_snap")
					if snapobj == undefined then continue
					WriteKey snapobj ((GetObjectId g_varSetBones[ i])-1) 2
				)
			TabStep 1;	format "}\n" to:Outfile
		)
		else
		(
			-- 일반적인 Keyfram 에니메이션 모델일때
			TabStep 1;	format "ANIMATIONGROUP id=0 name=\"ani_name\" count=%\n" (GetAnimationCount()) to:Outfile
			TabStep 1;	format "{\n" to:Outfile
				for i=1 to objects.count do
				(
					WriteKey objects[ i] ((GetObjectId objects[ i])-1) 2
				)
			TabStep 1;	format "}\n" to:Outfile
		)
		TabStep 0;	format "}\n" to:Outfile

		close Outfile
	)

	if 1 == g_BinaryWrite then
	(
		WriteBinaryFile()
	)

	if $ != undefined then deselect $
	select $'*snap'
	delete selection
)



rollout ui "냠냠" width:387 height:266
(
	button btnRun "DX Exporter 실행" pos:[101,108] width:177 height:30
	button btnOpenGL "OpenGL Exporter 실행" pos:[101,148] width:177 height:30
	spinner spnSamplingGap "" pos:[90,70] width:49 height:16 enabled:true range:[1,100,0] type:#integer scale:1 value:1
	label lbl39 "샘플링간격:" pos:[10,70] width:73 height:17

	-- Bmm, Ani 파일명 입력창
	checkbox chkBmm " " pos:[10,10] width:17 height:17 checked:true
	label lbl1 "Bmm 파일명:" pos:[30,13] width:70 height:17
	edittext editBmmFilePath "" pos:[100,10] width:250 height:17 enabled:true
	checkbox chkAni " " pos:[10,40] width:17 height:17 checked:true
	label lbl2 "Ani 파일명:" pos:[30,40] width:70 height:17
	edittext editAniFilePath "" pos:[100,40] width:250 height:17 enabled:true

	-- UI 창이 열릴때 
	on ui open do
	(
		editBmmFilePath.text = "bmm.txt"
		editAniFilePath.text = "ani.txt"		
	)

	on chkBmm changed state do
	(
		g_WriteBmm = state
	)
	on chkAni changed state do
	(
		g_WriteAni = state
	)

	-- DirectX Export
	on btnRun pressed do
	(
		g_ExporterType = 0
		if spnSamplingGap.value < 1 then spnSamplingGap.value = 1
		g_nSampling = spnSamplingGap.value
		g_BmmFilePath = editBmmFilePath.text
		g_AniFilePath = editAniFilePath.text

		ExporterStart()
	)

	-- OpenGL Export
	on btnOpenGL pressed do
	(
		g_ExporterType = 1
		if spnSamplingGap.value < 1 then spnSamplingGap.value = 1
		g_nSampling = spnSamplingGap.value
		g_BmmFilePath = editBmmFilePath.text
		g_AniFilePath = editAniFilePath.text

		ExporterStart()
	)

)

MainUI = newRolloutFloater "MAX to XML" 400 300
addrollout ui MainUI

