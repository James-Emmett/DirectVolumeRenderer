<Shader>
	<Properties>
		<Property type="Texture" name="SkyBox"/>
	</Properties>
	
	<RenderState>
		<BlendState>Opaque</BlendState>
		<DepthState>DepthDefault</DepthState>
		<RasterState>CullNone</RasterState>
	</RenderState>
	
	<Attributes>
		<Attribute name="POSITION" index="0" slotClass="PER_VERTEX" stepRate="0"/>
	</Attributes>
	
	<RenderQueue type="Geometry"/>
	
	<Shaders>
		<ShaderPath type="VS" source="Assets/Shaders/SkyBox/SkyBox.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/SkyBox/SkyBox.hlsl" entry="frag"/>
	</Shaders>
</Shader>