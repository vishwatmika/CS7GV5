using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StoneScript : MonoBehaviour
{
    public GameObject parentBone;
    public Rigidbody rigid;
    Vector3 lastPos;
    Vector3 curVel;

    // Start is called before the first frame update
    void Start()
    {
        transform.parent = parentBone.transform;
        rigid.useGravity = false;
    }

    // Update is called once per frame
    void Update()
    {

    }

    public void Release()
    {
        curVel = new Vector3(0, 1, 5);
        transform.parent = null;
        rigid.useGravity = true;
        transform.rotation = parentBone.transform.rotation;
        rigid.AddForce(curVel * 2, ForceMode.Impulse);
        Debug.Log("Throw");
    }
}
