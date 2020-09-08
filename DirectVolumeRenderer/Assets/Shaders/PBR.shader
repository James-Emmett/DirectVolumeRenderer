<Shader>
	<Properties>
		<Property type="Texture" name="Albedo"/>
		<Property type="Texture" name="Normal"/>
		<Property type="Texture" name="Metalness"/>
		<Property type="Texture" name="Roughness"/>
	</Properties>
	
	<RenderState>
		<BlendState>Opaque</BlendState>
		<DepthState>DepthDefault</DepthState>
		<RasterState>CullClockWise</RasterState>
	</RenderState>
	
	<Attributes>
		<Attribute name="POSITION" index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="NORMAL"   index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="TANGENT"  index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="COLOR"    index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="TEXCOORD" index="0" slotClass="PER_VERTEX" stepRate="0"/>
	</Attributes>
	
	<RenderQueue type="Geometry"/>
	
	<Shaders>
		<ShaderPath type="VS" source="Assets/Shaders/PBR.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/PBR.hlsl" entry="frag"/>
	</Shaders>
</Shader>