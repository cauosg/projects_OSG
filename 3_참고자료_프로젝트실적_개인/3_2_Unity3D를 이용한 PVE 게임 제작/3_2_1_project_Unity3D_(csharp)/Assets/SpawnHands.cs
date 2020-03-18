using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnHands : MonoBehaviour {
    public GameObject Player;
    public GameObject Rocks;
    public float Spawninterval = 2.0f;
    public int Spawnnums = 10;
    private float Spawncooltime = 0.0f;
    private bool isspawning = false;
    private int spawncount = 0;
    //private Vector3[] Spawnposes = new Vector3[Spawnnums];
    //private bool[] Spawnbools = new bool[Spawnnums];
    //private float[] eachcounts = new float[Spawnnums];
    // Use this for initialization
    void Start () {
        //for (int i = 0; i < Spawnnums; i++)
        //{
        //    eachcounts[i] = 0.0f;
        //    Spawnbools[i] = false;
        //}
        isspawning = false;
    }
	
	// Update is called once per frame
	void Update () {
        //if(Spawncooltime<0.1f)
        //if (spawncount < 1)
        //    isspawning = true;
        if (spawncount > Spawnnums)
        {
            spawncount = 0;
            Spawncooltime = 0;
            isspawning = false;
        }
        if (isspawning && Spawncooltime < Spawninterval/2 && spawncount<10)
        {
            //Spawnposes[spawncount] = Player.transform.position;
            spawncount++;
            Spawncooltime = Spawninterval;

            Instantiate(Rocks, Player.transform.position - Vector3.up*20 + Vector3.forward*3 + Vector3.left * 4.7f, Quaternion.identity);
        }
        //Spawncooltime -= Time.deltaTime;
        //if (isspawning)
        //{
        //    for (int i = 0; i < spawncount; i++)
        //        spawnrocks(Spawnposes[i]);
        //}
        //if (spawncount > 0)
        //{
        //    for(int i = 0)
        //}

        //Debug.Log(spawncount);
        if (Spawncooltime > 0)
            Spawncooltime -= Time.deltaTime;
    }
    public void setOn()
    {
        spawncount = 0;
        Spawncooltime = 0;
        isspawning = true;
    }
    //public void spawnrocks(Vector3 pos)
    //{
    //    Instantiate(Rocks, pos, Quaternion.identity);
    //}
}
