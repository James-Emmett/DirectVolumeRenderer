<Shader>
	<Properties>
		<Property name="Color" type="Vector3"/>
	</Properties>
	
	<RenderState>
		<BlendState>Opaque</BlendState>
		<DepthState>DepthDefault</DepthState>
		<RasterState>CullClockWise</RasterState>
		<Topology>LineList</Topology>
	</RenderState>
	
	<Attributes>
		<Attribute name="POSITION" index="0" slotClass="PER_VERTEX" stepRate="0"/>
	</Attributes>
	
	<RenderQueue type="Geometry"/>
	
	<Shaders>
		<ShaderPath type="VS" source="Assets/Shaders/Grid/grid.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/Grid/grid.hlsl" entry="frag"/>
	</Shaders>
</Shader>