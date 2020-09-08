<Shader>

	<Properties>
		<Property name="VolumeMap" type="Texture"/>
		<Property name="OccupancyMap" type="Texture"/>
		<Property name="Noise" type="Texture"/>
		<Property name="ScaleFactor" type="Vector3"/>
		<Property name="Hounsfield" type="Float" min="0" max="1.0"/>
		<Property name="StepSize" type="Vector3"/>
		<Property name="Iterations" type="Float" min="0" max="1.0"/>
		<Property name="VolumeDims" type="Vector3"/>
		<Property name="AlphaAmount" type="Float" min="0.001" max="1.0"/>
		<Property name="OccupancyDims" type="Vector3"/>
	</Properties>
	
	<RenderState>
		<BlendState>AlphaBlend</BlendState>
		<DepthState>DepthDefault</DepthState>
		<RasterState>CullClockwise</RasterState>
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
		<ShaderPath type="VS" source="Assets/Shaders/Volume/Alpha_Volume.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/Volume/Alpha_Volume.hlsl" entry="frag"/>
	</Shaders>
</Shader>