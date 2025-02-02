#base "default.res"

"classes/Medic_green.res"
{
	"classNameLabel"
	{
		"labelText"		"#TF_Medic"
	}
	
	"classInfo"
	{
		"text"			"#classinfo_medic"
	}
	
	"classModel"
	{
		"fov"			"23"
		
		"model"
		{
			"modelname"	"models/player/medic.mdl"
			"modelname_hwm"	"models/player/hwm/medic.mdl"
			"skin"		"8"
			"vcd"		"scenes/Player/Medic/low/class_select.vcd"
			
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_medigun.mdl"
				"skin"		"2"
			}
		}
	}
}