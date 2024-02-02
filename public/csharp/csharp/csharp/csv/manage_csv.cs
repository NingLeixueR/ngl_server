namespace ngl
{
    class manage_csv<TAB> where TAB : ICsvRead, ICsv, new()
    {
        public static Dictionary<Int32, TAB> tablecsv = new Dictionary<Int32, TAB>();

        public static bool load(string aname)
        {
            rcsv lcsv = new rcsv();
            Int32 lversion = 0;
            if (lcsv.read(aname, ref lversion) == false)
                return false;
            if (lcsv.readcsv(tablecsv) == false)
                return false;
            return true;
        }
    }
}