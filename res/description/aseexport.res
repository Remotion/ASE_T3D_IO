CONTAINER aseexport
{
	 NAME Faseexport;
	 INCLUDE Fbase;

	GROUP ID_FILTERPROPERTIES
	{
		STATICTEXT ASEE_INFO {}
		BOOL ASEE_REVERSENORMALS {}
		BOOL ASEE_SAVENORMALS {}
		BOOL ASEE_MATOBPREFIX {}
		BOOL ASEE_JOINOBJECTS {}
		LONG ASEE_COMMADIGITS {MIN 2; MAX 8;}
	}
}