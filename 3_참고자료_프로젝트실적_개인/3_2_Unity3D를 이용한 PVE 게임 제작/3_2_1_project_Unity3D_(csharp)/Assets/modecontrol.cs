using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class modecontrol : MonoBehaviour {
 
    public bool isbattle,once = true;
	// Use this for initialization
	void Start () {
		
	}
    private void OnTriggerEnter(Collider other)
    {
        if(once&&other.gameObject.tag == "bossgate")
        {
            isbattle = !isbattle;
            Debug.Log("event");
            Seton(isbattle);
            once = false;
        }
        if (other.gameObject.tag == "villagegate")
        {
            once = true;
            Debug.Log("event2");
        }
    }
    private void OnTriggerStay(Collider other)
    {
        if (once && other.gameObject.tag == "bossgate")
        {
            isbattle = !isbattle;
            Seton(isbattle);
            once = false;
            

        }
        if (other.gameObject.tag == "villagegate")
        {
            once = true;
            Debug.Log("event2");
        }
    }
        //private void OnTriggerExit(Collider other)
        //{
        //    if (once && other.gameObject.tag == "bossgate")
        //    {
        //        once = true;
        //        Debug.Log("event2");
        //    }
        //}
        // Update is called once per frame
        void Update () {
		
	}
    private void Seton(bool k)
    {
        GameObject.Find("boss_golem").GetComponent<patternmanage>().Setact(k);
        GameObject.Find("fallings").GetComponent<falls>().Setact(k);
    }

}
