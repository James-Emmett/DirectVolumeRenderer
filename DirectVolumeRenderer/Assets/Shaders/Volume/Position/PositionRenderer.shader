<Shader>
	<RenderState>
		<BlendState>AlphaBlend</BlendState>
		<DepthState>DepthNone</DepthState>
		<RasterState>CullNone</RasterState>
	</RenderState>
	
	<Attributes>
		<Attribute name="POSITION" index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="NORMAL"   index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="TANGENT"  index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="COLOR"    index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="TEXCOORD" index="0" slotClass="PER_VERTEX" stepRate="0"/>
	</Attributes>
	
	<RenderQueue type="Transparent"/>
	
	<Shaders>
		<ShaderPath type="VS" source="Assets/Shaders/Volume/Position/PositionRender.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/Volume/Position/PositionRender.hlsl" entry="frag"/>
	</Shaders>
</Shader>